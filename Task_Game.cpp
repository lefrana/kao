//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include  "Task_Ending.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		srand((unsigned int)time(NULL));
		this->isP1Over = false;

		//★タスクの生成

		//Face Parts Initialize
		if (g_playerCount == 1)
		{
			fp = FaceParts::Object::Create(true);
		}
		else
		{
			p1 = Player1::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("本編");
		ge->KillAll_G("FaceParts");
		ge->KillAll_G("Player1");
		ge->KillAll_G("Player2");

		if (!ge->QuitFlag() && this->nextTaskCreate) 
		{
			//★引き継ぎタスクの生成
			auto next = Ending::Object::Create(true);

			if (g_playerCount == 1)
			{
				next->score = fp->score;

				next->fpData.lefteyeY = fp->lefteye.newPosY;
				next->fpData.righteyeY = fp->righteye.newPosY;
				next->fpData.noseY = fp->nose.newPosY;
				next->fpData.mouthY = fp->mouth.newPosY;
			}

			else
			{
				next->scoreP1 = p1->score;
				next->scoreP2 = p2->score;

				if (p1->score.Totalpoints >= p2->score.Totalpoints)
				{
					next->fpData.lefteyeY	= p1->lefteye.newPosY;
					next->fpData.righteyeY	= p1->righteye.newPosY;
					next->fpData.noseY		= p1->nose.newPosY;
					next->fpData.mouthY		= p1->mouth.newPosY;
					next->winner = 1;
				}
				else
				{
					next->fpData.lefteyeY	= p2->lefteye.newPosY;
					next->fpData.righteyeY	= p2->righteye.newPosY;
					next->fpData.noseY		= p2->nose.newPosY;
					next->fpData.mouthY		= p2->mouth.newPosY;
					next->winner = 2;
				}
			}
		}
		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//if(obj->)
		auto inp = ge->in1->GetState( );

		if (g_playerCount == 1)
		{
			if (fp && inp.B1.down && fp->IsAllStopped())
			{
				this->Kill();
			}
		}
		/*else
		{
			if (inp.B2.down)
			{
				this->Kill();
			}
		}*/

		else if (g_playerCount == 2)
		{
			if (!isP1Over)
			{
				if (p1 && inp.B2.down && p1->IsAllStopped())
				{
					isP1Over = true;
					p2 = Player2::Object::Create(true);
				}
			}
			else
			{
				if (p2 && inp.B2.down && p2->IsAllStopped())
				{
					this->Kill();
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
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