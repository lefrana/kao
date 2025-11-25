#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//Face Parts
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  FaceParts
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("FaceParts");	//グループ名
	const  string  defName("NoName");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
		DG::Image::SP	img;
		DG::Image::SP	imgBody;
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		//ML::Point		pos;
		//ML::Box2D		hitBase;
		int				logoPosX;

		XI::GamePad::SP	controller;

		//bool	isMovingDown;
		//bool	isStopping;

		enum class State { Normal, Move, Stop };

		struct FacePart 
		{
			State			state;
			ML::Point		pos;
			bool			isMovingDown;
			float			newPosY;
		};

		FacePart	lefteye, righteye, nose, mouth;

		struct Score
		{
			float			Totalpoints;
			bool			isGood;
		};

		static Score score;

		void  GetScore();

		void  FacePart_Initialize(FacePart& p_, int x_);
		void  FacePart_UpDate(FacePart& p_);
		void  FacePart_Draw(FacePart& p_, int sx_, int sw_, int w_);

		bool  IsAllStopped();


	};
}