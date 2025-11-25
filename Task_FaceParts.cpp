//-------------------------------------------------------------------
//Face Parts
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_FaceParts.h"
#include  "sound.h"

#include  <iostream>

FaceParts::Object::Score FaceParts::Object::score = { 0.f, false };

namespace  FaceParts
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/kao_faceparts.png");
		this->imgBody = DG::Image::Create("./data/image/kao_body.png");
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
		this->logoPosX = 480;

		this->render2D_Priority[1] = 0.5f;
		this->controller = ge->in1;

		se::LoadFile("click", "./data/sound/se/click.wav");

		FacePart_Initialize(this->lefteye, 190);
		FacePart_Initialize(this->righteye,260);
		FacePart_Initialize(this->mouth, 240 - 32);
		FacePart_Initialize(this->nose, 240 - 16);

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
		this->logoPosX -= 3;
		if (this->logoPosX <= 0) 
		{
			this->logoPosX = 0;
		}

		if (logoPosX == 0)
		{ 
			if (this->lefteye.state != State::Stop)
			{
				FacePart_UpDate(this->lefteye);
			}
			else if (this->righteye.state != State::Stop)
			{
				FacePart_UpDate(this->righteye);
			}
			else if (this->nose.state != State::Stop)
			{
				FacePart_UpDate(this->nose);
			}
			else if (this->mouth.state != State::Stop)
			{
				FacePart_UpDate(this->mouth);
			}


			GetScore();

			if (this->score.isGood)
			{
				std::cout << "good" << this->score.Totalpoints << std::endl;
			}
			else
			{
				std::cout << "bad" << this->score.Totalpoints << std::endl;
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D	draw(480/3, 270/4, 160, 256);
		ML::Box2D	src(0, 0, 160, 256);
		draw.Offset(this->logoPosX, 0);
		this->res->imgBody->Draw(draw, src);

		////left eye check
		//draw=ML::Box2D(190, 140, 32,32);
		//src=ML::Box2D(0, 0, 32, 32);
		//this->res->img->Draw(draw, src);
		////right eye check
		//draw = ML::Box2D(260, 140, 32,32);
		//src = ML::Box2D(0, 0, 32, 32);
		//this->res->img->Draw(draw, src);
		////mouth check
		//draw = ML::Box2D(208, 180, 64, 32);
		//src = ML::Box2D(64, 0, 64, 32);
		//this->res->img->Draw(draw, src);
		////nose check
		//draw = ML::Box2D(224, 165, 32, 32);
		//src = ML::Box2D(32, 0, 32, 32);
		//this->res->img->Draw(draw, src);


		FacePart_Draw(this->lefteye, 32 * 0, 32, 0);
		FacePart_Draw(this->righteye, 32 * 0, 32, 0);
		FacePart_Draw(this->mouth, 32 * 2, 64, 32);
		FacePart_Draw(this->nose, 32 * 1, 32, 0);
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
			p_.pos.y = rand()%(270 - 32);
			p_.state = State::Move;
		}

		if (p_.state == State::Move)
		{
			if (p_.isMovingDown)
			{
				p_.pos.y += 3;
				if (p_.pos.y >= 270 - 32)
				{
					p_.isMovingDown = false;
				}
			}
			else
			{
				p_.pos.y -= 3;
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
				se::Play("click");
				p_.newPosY = p_.pos.y;
			}
		}
	}
	//-------------------------------------------------------------------
	void  Object::GetScore()
	{
		//calculate points based on sum of pos.y
		this->score.Totalpoints = lefteye.newPosY + righteye.newPosY +
			nose.newPosY + mouth.newPosY;

		//mark as good if total is in range
		score.isGood = (this->score.Totalpoints >= 585.f && this->score.Totalpoints <= 635.f);
	}
	//-------------------------------------------------------------------
	//checking for ending flag
	bool  Object::IsAllStopped()
	{
		return
			this->lefteye.state		== State::Stop &&
			this->righteye.state	== State::Stop &&
			this->nose.state		== State::Stop &&
			this->mouth.state		== State::Stop;
	}
	//-------------------------------------------------------------------
	void  Object::FacePart_Draw(FacePart& p_, int sx_, int sw_, int w_)
	{
		ML::Box2D	draw(0, 0, 32 + w_, 32);
		draw.Offset(p_.pos);

		ML::Box2D	src(sx_, 0, sw_, 32);
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