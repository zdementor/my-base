//---------------------------------------------------------------------------

#ifndef fMainFormH
#define fMainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <Menus.hpp>

//! 8 bit unsigned variable.
//! This is a typedef for unsigned char, it ensures portability of the engine.
typedef unsigned char       u8;

//----------------------------------------------------------------------------

//! 8 bit signed variable.
//! This is a typedef for signed char, it ensures portability of the engine.
typedef signed char         s8;

//----------------------------------------------------------------------------

//! 8 bit character variable.
//! This is a typedef for char, it ensures portability of the engine.
typedef char                c8;

//----------------------------------------------------------------------------

//! 16 bit unsigned variable.
//! This is a typedef for unsigned short, it ensures portability of the engine. 
typedef unsigned short      u16;

//----------------------------------------------------------------------------

//! 16 bit signed variable.
//! This is a typedef for signed short, it ensures portability of the engine. 
typedef signed short        s16;

//----------------------------------------------------------------------------

//! 32 bit unsigned variable.
//! This is a typedef for unsigned int, it ensures portability of the engine.
typedef unsigned int        u32;

//----------------------------------------------------------------------------

//! 32 bit signed variable.
//! This is a typedef for signed int, it ensures portability of the engine. 
typedef signed int          s32;

//----------------------------------------------------------------------------

//! 64 bit signed variable.
//! This is a typedef for __int64, it ensures portability of the engine.
typedef __int64             s64;

//----------------------------------------------------------------------------

//! 32 bit floating point variable.
//! This is a typedef for float, it ensures portability of the engine.
typedef float               f32;

//----------------------------------------------------------------------------

//! 64 bit floating point variable.
//! This is a typedef for double, it ensures portability of the engine.

typedef double              f64;


#define SAFE_DELETE(p)             { if (p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)       { if (p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)            { if (p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DROP(p)               { if (p) { (p)->drop();    (p)=NULL; } }
#define SAFE_GRAB(p)               { if (p) { (p)->grab(); } }
#define SAFE_CALL(p,f)             { if (p) { (p)->f; } }

#define CHECK_RANGE(v, rmin, rmax) { if (v<rmin) v=rmin; else if (v>rmax) v=rmax; }
#define CHECK_MIN_RANGE(v, rmin)   { if (v<rmin) v=rmin; }
#define CHECK_MAX_RANGE(v, rmax)   { if (v>rmax) v=rmax; }
#define CHECK_NO_ZERO(v)           { if (v==0) v=0.0001; }


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *File1;
    TMenuItem *OpenMap1;
    TMenuItem *N1;
    TMenuItem *Exit1;
    TOpenPictureDialog *OpenPictureDialog1;
    TGroupBox *GroupBox1;
    TMemo *Memo1;
    TPanel *Panel1;
    TImage *Image1;
    TImage *Image2;
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall OpenMap1Click(TObject *Sender);
    void __fastcall Image1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TMainForm(TComponent* Owner);

    int startX, startY;
    int endX, endY;

    int FindPath();

     bool HighPerformanceTimerSupport;
     double performanceUnit;
     double performanceCounterToMs;

    class CPathfinder
	{
	public:

		CPathfinder() 
			: mapWidth(0), mapHeight(0), onClosedList(10),
		walkability(NULL), influence(0), heightinfluence(0), openList(NULL), whichList(NULL), 
		openX(NULL), openY(NULL), parentX(NULL), parentY(NULL), 
		Fcost(NULL), Gcost(NULL), Hcost(NULL), pathBank(NULL) {}
			
		int mapWidth, mapHeight; 
		int onClosedList;

		static const s32 notfinished;
		static const s32 notStarted;
		static const s32 found;
		static const s32 nonexistent; 
		static const s32 walkable;
		static const s32 unwalkable;

		// needed arrays
		u8 **walkability, **influence;
		u16 **heightinfluence;

		int *openList;    //1 dimensional array holding ID# of open list items
		int **whichList;  //2 dimensional array used to record 

		// whether a cell is on the open list or on the closed list.
		int *openX;    //1d array stores the x location of an item on the open list
		int *openY;    //1d array stores the y location of an item on the open list
		int **parentX; //2d array to store parent of each cell (x)
		int **parentY; //2d array to store parent of each cell (y)
		int *Fcost;	  //1d array to store F cost of a cell on the open list
		int **Gcost;   //2d array to store G cost for each cell.
		int *Hcost;    //1d array to store H cost of a cell on the open list	
		int *pathBank; //stores the founded path array

		int pathLength;   //stores length of the found path for critter
		int pathLocation; //stores current position along the chosen path for critter	

		//Path reading variables
		int pathStatus, xPath, yPath;

		// Allocates memory for the pathfinder.
		void InitializePathfinder(int width, int height);

		// Frees memory used by the pathfinder.
		void DeinitializePathfinder();

		// Finds a path using A*
		int FindPath( int startingX, int startingY, int targetX, int targetY );

		void swapWalkabilityBuffers();

		void mapWalkabilityBuffer(s32 x, s32 y, u8 weight);

		bool isWalkable(s32 x, s32 y);

		u8 getInfluence(s32 x, s32 y);	
		u16 getHeightInfluence(s32 x, s32 y);	
	}
	Pathfinder;
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
