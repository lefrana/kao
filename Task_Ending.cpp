//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "sound.h"
#include  "Task_FaceParts.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgCat = DG::Image::Create("./data/image/kao_body.png");
		this->imgTextGood = DG::Image::Create("./data/image/good_text.png");
		this->imgTextBad = DG::Image::Create("./data/image/bad_text.png");
		this->imgFace = DG::Image::Create("./data/image/kao_faceparts.png");

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgCat.reset();
		this->imgFace.reset();
		this->imgTextGood.reset();
		this->imgTextBad.reset();
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
		this->fade = 0.f;
		this->time = 0;
		this->textAnim = 0;

		se::LoadFile("goodending", "./data/sound/se/goodending.wav");
		se::LoadFile("badending", "./data/sound/se/badending.wav");
		this->speechPlayed = false;



		//★タスクの生成
		fp = FaceParts::Object::Create(false);


		//fp->FacePart_UpDate(fp->lefteye);
		//fp->FacePart_UpDate(fp->righteye);
		//fp->FacePart_UpDate(fp->nose);
		//fp->FacePart_UpDate(fp->mouth);

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		this->time++;

		//cat fading in
		this->fade += 0.005f;

		if (this->fade > 1.0f)
		{
			this->fade = 1.0f;
		}

		if (this->fade >= 1.0f && !this->speechPlayed)
		{
			if (FaceParts::Object::score.isGood)
			{
				se::Play("goodending");
			}
			else
			{
				se::Play("badending");
			}
			this->speechPlayed = true; //speech only plays once
		}

		if (this->fade >= 1.0f)
		{
			//for text animation
			if (this->time % 10 == 0)
			{
				this->textAnim++;
			}

			if (FaceParts::Object::score.isGood)
			{
				if (this->textAnim >= 9)
				{
					this->textAnim = 9; //good ending

					if (inp.ST.down)
					{
						//自身に消滅要請
						this->Kill();
					}
				}
			}

			else
			{
				if (this->textAnim >= 7)
				{
					this->textAnim = 7; //good ending

					if (inp.ST.down)
					{
						//自身に消滅要請
						this->Kill();
					}
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D drawCat(50, 10, 160, 256);
		ML::Box2D srcCat(0, 0, 160, 256);

		ML::Color col(this->fade, 1.f, 1.f, 1.f);
		this->res->imgCat->Draw(drawCat, srcCat, col);

		{
			//draw left eye
			ML::Box2D drawFace(fp->lefteye.pos.x - 110, fp->lefteye.newPosY - 57, 32, 32);
			ML::Box2D srcFace(0, 0, 32, 32);
			this->res->imgFace->Draw(drawFace, srcFace, col);

			//right eye
			drawFace = ML::Box2D(fp->righteye.pos.x - 110, fp->righteye.newPosY - 57, 32, 32);
			this->res->imgFace->Draw(drawFace, srcFace, col);

			//nose
			drawFace = ML::Box2D(fp->nose.pos.x - 110, fp->nose.newPosY - 57, 32, 32);
			srcFace.x += 32;
			this->res->imgFace->Draw(drawFace, srcFace, col);

			//mouth
			drawFace = ML::Box2D(fp->mouth.pos.x - 110, fp->mouth.newPosY - 57, 64, 32);
			srcFace.x += 32;
			srcFace.w += 32;
			this->res->imgFace->Draw(drawFace, srcFace, col);
		}

		if (FaceParts::Object::score.isGood)
		{
			ML::Box2D drawText(220, 60, 135, 60);
			ML::Box2D srcText(0, 60 * this->textAnim, 135, 60);
			this->res->imgTextGood->Draw(drawText, srcText);
		}
		else
		{
			ML::Box2D drawText(220, 30, 150, 30);
			ML::Box2D srcText(0, 30 * this->textAnim, 150, 30);
			this->res->imgTextBad->Draw(drawText, srcText);
		}
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