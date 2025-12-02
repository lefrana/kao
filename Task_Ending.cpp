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
		this->imgEnd = DG::Image::Create("./data/image/fin.png");

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
		this->imgEnd.reset();

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
		this->fadeCat = 0.f;
		this->fadeEnd = 0.f;
		this->time = 0;
		this->textAnim = 0;

		se::LoadFile("goodending", "./data/sound/se/goodending.wav");
		se::LoadFile("badending", "./data/sound/se/badending.wav");
		this->speechPlayed = false;



		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		se::Stop("badending");
		se::Stop("goodending");


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
		this->fadeCat += 0.005f;

		if (this->fadeCat > 1.0f)
		{
			this->fadeCat = 1.0f;
		}

		if (this->fadeCat >= 1.0f && !this->speechPlayed)
		{
			if (g_playerCount == 1)
			{
				if (this->score.isGood)
				{
					se::Play("goodending");
				}
				else
				{
					se::Play("badending");
				}
			}
			else
			{
				se::Play("goodending");
			}
			this->speechPlayed = true;
		}

		if (this->fadeCat >= 1.0f)
		{
			//for text animation
			if (this->time % 10 == 0)
			{
				this->textAnim++;
			}

			if (g_playerCount == 1 && this->score.isGood || g_playerCount == 2)
			{
				if (this->textAnim >= 9)
				{
					this->textAnim = 9; //good ending

					this->fadeEnd += 0.003f;

					if (this->fadeEnd > 1.0f)
					{
						this->fadeEnd = 1.0f;

						if (inp.B1.down || inp.B2.down)
						{
							this->Kill();
						}
					}
				}
			}

			else if (g_playerCount == 1 && !this->score.isGood)
			{
				if (this->textAnim >= 7) //bad ending p1
				{
					this->textAnim = 7;

					this->fadeEnd += 0.003f;

					if (this->fadeEnd > 1.0f)
					{
						this->fadeEnd = 1.0f;

						if (inp.B1.down)
						{
							this->Kill();
						}
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
		ML::Color colCat;
		if (g_playerCount == 1)
		{
			colCat = ML::Color(this->fadeCat, 1.f, 1.f, 1.f);
		}
		else
		{
			if (this->winner == 1)
			{
				colCat = ML::Color(this->fadeCat, 1.f, .7f, .9f);
			}
			else
			{
				colCat = ML::Color(this->fadeCat, 1.f, .7f, .3f);
			}
		}

		this->res->imgCat->Draw(drawCat, srcCat, colCat);

		{
			//draw left eye
			ML::Box2D drawFace(190 - 110, this->fpData.lefteyeY - 57, 32, 32);
			ML::Box2D srcFace(0, 0, 32, 32);
			this->res->imgFace->Draw(drawFace, srcFace, colCat);

			//right eye
			drawFace = ML::Box2D(260 - 110, this->fpData.righteyeY - 57, 32, 32);
			this->res->imgFace->Draw(drawFace, srcFace, colCat);

			//mouth
			drawFace = ML::Box2D(240 - 32 - 110, this->fpData.mouthY - 57, 64, 32);
			srcFace.x += 64;
			srcFace.w += 32;
			this->res->imgFace->Draw(drawFace, srcFace, colCat);

			//nose
			drawFace = ML::Box2D(240 - 16 - 110, this->fpData.noseY - 57, 32, 32);
			srcFace.x -= 32;
			srcFace.w -= 32;
			this->res->imgFace->Draw(drawFace, srcFace, colCat);
		}

		if (g_playerCount == 1 && this->score.isGood || g_playerCount == 2) //good ending n p2
		{
			ML::Box2D drawText(220, 60, 135, 60);
			ML::Box2D srcText(0, 60 * this->textAnim, 135, 60);
			this->res->imgTextGood->Draw(drawText, srcText);
		}

		else if (g_playerCount == 1 && !this->score.isGood) //bad ending for p1
		{
			ML::Box2D drawText(220, 60, 150, 30);
			ML::Box2D srcText(0, 30 * this->textAnim, 150, 30);
			this->res->imgTextBad->Draw(drawText, srcText);
		}

		ML::Box2D drawEnd(345, 202, 110, 60);
		ML::Box2D srcEnd(0, 0, 110, 60);
		ML::Color colEnd(this->fadeEnd, 1.f, 1.f, 1.f);
		this->res->imgEnd->Draw(drawEnd, srcEnd, colEnd);
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