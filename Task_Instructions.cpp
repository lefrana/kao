//-------------------------------------------------------------------
//Istructions
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Instructions.h"
#include  "Task_Game.h"
#include  "sound.h"


namespace  Instructions
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgCat = DG::Image::Create("./data/image/kao_body.png");
		this->imgText = DG::Image::Create("./data/image/instruction_text.png");


		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgCat.reset();
		this->imgText.reset();
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

		se::LoadFile("speech", "./data/sound/se/speech.wav");
		this->speechPlayed = false;
	
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) 
		{
			//★引き継ぎタスクの生成
			auto  nextTask = Game::Object::Create(true);
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
			se::Play("speech");
			this->speechPlayed = true; //speech only plays once
		}

		if (this->fade >= 1.0f)
		{
			//for text animation
			if (this->time % 10 == 0)
			{
				this->textAnim++;
			}

			if (this->textAnim >= 12)
			{
				this->textAnim = 12;

				if (inp.ST.down)
				{
					//自身に消滅要請
					this->Kill();
				}
			}
		}
		//se::EndCheck();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D drawCat(50, 10, 160, 256);
		ML::Box2D srcCat(0, 0, 160, 256);

		ML::Color col(this->fade, 1.f, 1.f, 1.f);
		this->res->imgCat->Draw(drawCat, srcCat, col);

		ML::Box2D drawText(220, 60, 215, 70);
		ML::Box2D srcText(0, 70 * this->textAnim, 215, 70);
		this->res->imgText->Draw(drawText, srcText);
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