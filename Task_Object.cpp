//-------------------------------------------------------------------
//Object
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Object.h"

namespace  Object00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/HitTest.bmp");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		//this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->render2D_Priority[1] = 0.5f;
		this->controller = ge->in1;
		/*this->pos.x = 0;
		this->pos.y = 0;

		this->isMovingDown = true;
		this->isStopping = false;*/

		FacePart_Initialize(this->lefteye, 480 / 3);
		FacePart_Initialize(this->righteye,480/3+200);
		FacePart_Initialize(this->nose,480/3+100);
		FacePart_Initialize(this->mouth,480/3+132);


		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		if (this->lefteye.state != State::Stop)
		{
			FacePart_UpDate(this->lefteye);
		}
		// 2. If Left Eye stopped, update Right Eye
		// 2. 左目が止まっていたら、右目を更新
		else if (this->righteye.state != State::Stop)
		{
			FacePart_UpDate(this->righteye);
		}
		// 3. If Right Eye stopped, update Nose
		// 3. 右目が止まっていたら、鼻を更新
		else if (this->nose.state != State::Stop)
		{
			FacePart_UpDate(this->nose);
		}
		// 4. If Nose stopped, update Mouth
		// 4. 鼻が止まっていたら、口を更新
		else if (this->mouth.state != State::Stop)
		{
			FacePart_UpDate(this->mouth);
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		FacePart_Draw(this->lefteye);
		FacePart_Draw(this->righteye);
		FacePart_Draw(this->nose);
		FacePart_Draw(this->mouth);
	}
	//-------------------------------------------------------------------
	void  Object::FacePart_Initialize(FacePart& p_, int x_)
	{
		p_.state = State::Normal;
		p_.pos.x = x_;
		p_.pos.y = -100;
		p_.isMovingDown = true;
	}
	//-------------------------------------------------------------------
	void  Object::FacePart_UpDate(FacePart& p_)
	{
		if (p_.state == State::Normal)
		{
			p_.pos.y = rand()%(270 - 64);
			p_.state = State::Move;
		}

		if (p_.state == State::Move)
		{
			if (p_.isMovingDown)
			{
				p_.pos.y += 5;
				if (p_.pos.y >= 270 - 64)
				{
					p_.isMovingDown = false;
				}
			}
			else
			{
				p_.pos.y -= 5;
				if (p_.pos.y <= 0)
				{
					p_.pos.y = 0;
					p_.isMovingDown = true;
				}
			}
		}

		if (this->controller)
		{
			auto inp = this->controller->GetState();
			if (inp.B1.down)
			{
				p_.state = State::Stop;
			}
		}
	}
	//-------------------------------------------------------------------
	void  Object::FacePart_Draw(FacePart& p_)
	{
		ML::Box2D	draw(0, 0, 64, 64);
		draw.Offset(p_.pos);
		ML::Box2D	src(0, 0, 100, 100);
		this->res->img->Draw(draw, src);
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}