/*
 * backgammon.h
 *
 * by Gary Wong <gtw@gnu.org>, 1999, 2000, 2001, 2002.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

#ifndef _BACKGAMMON_H_
#define _BACKGAMMON_H_


#include <float.h>
#include <list.h>
#include <math.h>
#include <stdarg.h>

#include "analysis.h"
#include "eval.h"

#if USE_TIMECONTROL && WIN32
#include <winsock2.h>
#endif

#if !defined (__GNUC__) && !defined (__attribute__)
#define __attribute__(X)
#endif

#ifdef HUGE_VALF
#define ERR_VAL (-HUGE_VALF)
#elif defined (HUGE_VAL)
#define ERR_VAL (-HUGE_VAL)
#else
#define ERR_VAL (-FLT_MAX)
#endif

#if USE_GTK
#include <gtk/gtk.h>
extern GtkWidget* pwBoard;
extern int fX, nDelay, fNeedPrompt;
extern guint nNextTurn; /* GTK idle function */
#if USE_TIMECONTROL
extern void GTKUpdateClock(void);
#endif
#elif USE_EXT
#include <ext.h>
#include <event.h>
extern extwindow ewnd;
extern int fX, nDelay, fNeedPrompt;
extern event evNextTurn;
#endif

#if HAVE_SIGACTION
typedef struct sigaction psighandler;
#elif HAVE_SIGVEC
typedef struct sigvec psighandler;
#else
typedef RETSIGTYPE (*psighandler)( int );
#endif

#define MAX_CUBE ( 1 << 12 )

#ifdef WIN32
#include <stdlib.h>
#define BIG_PATH _MAX_PATH
#else
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#define BIG_PATH PATH_MAX
#endif

/* position of windows: main window, game list, and annotation */

typedef enum _gnubgwindow {
  WINDOW_MAIN = 0,
  WINDOW_GAME,
  WINDOW_ANNOTATION,
  WINDOW_HINT,
  WINDOW_MESSAGE,
  NUM_WINDOWS 
} gnubgwindow;

typedef struct _windowgeometry {
#if USE_GTK
  gint nWidth, nHeight;
  gint nPosX, nPosY;
#else
  int nWidth, nHeight;
  int nPosX, nPosY;
#endif
} windowgeometry;

/* predefined board designs */

extern windowgeometry awg[ NUM_WINDOWS ];


typedef struct _monitor {
#if USE_GTK
    int fGrab;
    int idSignal;
#else
	int dummy;
#endif
} monitor;

typedef struct _command {
  /* Command name (NULL indicates end of list) */
  char* sz; 
  
  /* Command handler; NULL to use default subcommand handler */
  void ( *pf )( char* ); 

  /* Documentation; NULL for abbreviations */
  char* szHelp;
  char* szUsage; 

  /* List of subcommands (NULL if none) */
  struct _command* pc; 
} command;

typedef enum _playertype {
    PLAYER_EXTERNAL, PLAYER_HUMAN, PLAYER_GNU, PLAYER_PUBEVAL
} playertype;

typedef struct _player {
  /* For all player types: */
  char szName[ 32 ];

  playertype pt;

  /* For PLAYER_GNU: */
  evalsetup esChequer, esCube;
  movefilter aamf[ MAX_FILTER_PLIES ][ MAX_FILTER_PLIES ];
  int h;
  /* For PLAYER_EXTERNAL: */
  char* szSocket;
} player;

typedef enum _movetype {
    MOVE_GAMEINFO,
    MOVE_NORMAL,
    MOVE_DOUBLE,
    MOVE_TAKE,
    MOVE_DROP,
    MOVE_RESIGN,
    MOVE_SETBOARD,
    MOVE_SETDICE,
    MOVE_SETCUBEVAL,
    MOVE_SETCUBEPOS,
    MOVE_TIME
} movetype;

#ifdef USE_TIMECONTROL

#include <time.h>
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* The different timing types
 * TC_PLAIN - normal chess clock timing
 * TC_FISCHER - Fischer clock - a fixed time is added to clock each move
 * TC_BRONSTEIN - Bronstein (FIDE) clock - The player gets a little "free"
 *	time each move, before the clock starts, but no time is added.
 * TC_HOURGLASS - Hourglass timing, what player A spends is added to B's
 *	clock and vice versa
 * TC_UNKNOWN - unknown timing type (e.g. read from file)
 */
typedef enum _tctiming {
TC_NONE=0, TC_PLAIN, TC_FISCHER, TC_BRONSTEIN, TC_HOURGLASS, TC_UNKNOWN
} tctiming;

/* The different penalty types
 * TC_POINT - a number of points - possibly 0 if there is no penalty -
 *	 is added to the other players score
 * TC_LOSS - the player loses the match
 */
typedef enum _tcpenalty {
TC_POINT, TC_LOSS
} tcpenalty;

typedef struct _timecontrol {
    char *szName;
    tctiming timing;
    tcpenalty penalty;

    int nAddedTime;		/* A fixed time to add at the start of this
	time control */
    int nPointAllowance; 	/* Time to add per point remaining in match */
    double dMultiplier; 	/* How much of old time allowance to keep when
	starting this time control */
    int nMoveAllowance;		/* Time allowance per move */
    int nPenalty;		/* Point penalty for timing out.  May be 0. */

    char *szNext;
	/* Next time control for the player whose time runs out. 
	   NULL means reiterate same */
    char *szNextB; 
	/* The other guy's next time control
	   NULL means no change to his time control*/
} timecontrol;
   
typedef struct _tcnode {
    timecontrol *ptc;
    struct _tcnode *next;
} tcnode;

typedef struct _playerclock {
    struct timeval tvStamp; 
    struct timeval tvTimeleft;
    timecontrol tc;
   /*  int nTimeouts; */
} playerclock;
   
typedef struct _gameclock {
   playerclock pc[2];
   struct timeval pausedtime;
   int fPaused; 
} gameclock;
    
#endif

typedef struct _movegameinfo {
  /* "standard" header */
  movetype mt;
  char* sz;
#if USE_TIMECONTROL
  /* time left after the move */
  struct timeval tl[2];
#endif

  /* ordinal number of the game within a match */
  int i;

  /* match length */  
  int nMatch;

  /* match score BEFORE the game */
  int anScore[2];
  
  /* the Crawford rule applies during this match */
  int fCrawford;

  /* this is the Crawford game */
  int fCrawfordGame;

  int fJacoby;

  /* who won (-1 = unfinished) */
  int fWinner;

  /* how many points were scored by the winner */
  int nPoints;

  /* the game was ended by resignation */
  int fResigned;

  /* how many automatic doubles were rolled */
  int nAutoDoubles; 

  /* Type of game */ 
  bgvariation bgv;

  /* Cube used in game */
  int fCubeUse;

  statcontext sc;
#if USE_TIMECONTROL
  /* the game ended on time */ 
    int fTimeout; 
  /* how many timeouts (clock expiry) have happened during the match */
    int nTimeouts[2];
#endif
} movegameinfo;

typedef struct cubedecisiondata {
  float aarOutput[ 2 ][ NUM_ROLLOUT_OUTPUTS ];
  float aarStdDev[ 2 ][ NUM_ROLLOUT_OUTPUTS ];
  evalsetup esDouble;
} cubedecisiondata;

typedef struct _movedouble {
    movetype mt;
    char* sz;
#if USE_TIMECONTROL
    struct timeval tl[2];
#endif
    int fPlayer;
    int nAnimals;    /* 0 in match play, even numbers are doubles, raccoons
                        odd numbers are beavers, aardvarken, etc. */
    cubedecisiondata* CubeDecPtr;
    cubedecisiondata  CubeDec;
    skilltype st;
} movedouble;

typedef struct _movenormal {
  /* "standard" header */
  movetype mt;
  char* sz;
#if USE_TIMECONTROL
  struct timeval tl[2];
#endif

  int fPlayer;

  /* dice for move */ 
  int anRoll[2];

  /* Move made. */
  int anMove[ 8 ];

  /* evaluation setup for move analysis */
  evalsetup esChequer;

  /* evaluation of cube action before this move */
  float aarOutput[ 2 ][ NUM_ROLLOUT_OUTPUTS ];
  float aarStdDev[ 2 ][ NUM_ROLLOUT_OUTPUTS ];

  evalsetup esDouble;

  /* evaluation of the moves */
  movelist ml;

  /* index into the movelist of the move that was made */
  int iMove; 

  lucktype lt;

  /* ERR_VAL means unknown */
  float rLuck; 

  skilltype stMove;
  skilltype stCube;
} movenormal;

typedef struct _moveresign {
    movetype mt;
    char* sz;
#if USE_TIMECONTROL
    struct timeval tl[2];
#endif
    int fPlayer;
    int nResigned;

    evalsetup esResign;
    float arResign[ NUM_ROLLOUT_OUTPUTS ];

    skilltype stResign;
    skilltype stAccept;
} moveresign;

typedef struct _movesetboard {
    movetype mt;
    char* sz;
#if USE_TIMECONTROL
    struct timeval tl[2];
#endif
    unsigned char auchKey[ 10 ]; /* always stored as if player 0 was on roll */
} movesetboard;

typedef struct _movesetdice {
    movetype mt;
    char* sz;
#if USE_TIMECONTROL
    struct timeval tl[2];
#endif
    int fPlayer;
    int anDice[ 2 ];
    lucktype lt;
    float rLuck; /* ERR_VAL means unknown */
} movesetdice;

typedef struct _movesetcubeval {
    movetype mt;
    char* sz;
#if USE_TIMECONTROL
    struct timeval tl[2];
#endif
    int nCube;
} movesetcubeval;

typedef struct _movesetcubepos {
    movetype mt;
    char *sz;
#if USE_TIMECONTROL
    struct timeval tl[2];
#endif
    int fCubeOwner;
} movesetcubepos;

#if USE_TIMECONTROL
typedef struct _movetime {
    movetype mt;
    char *sz;
    struct timeval tl[2];
    int fPlayer;
    int nPoints;
} movetime;
#endif

typedef union _moverecord {
    movetype mt;
    struct _moverecordall {
	movetype mt;
	char* sz;
#if USE_TIMECONTROL
	struct timeval tl[2];
#endif
    } a;
    movegameinfo g;
    movedouble d; /* cube decisions */
    movenormal n;
    moveresign r;
    movesetboard sb;
    movesetdice sd;
    movesetcubeval scv;
    movesetcubepos scp;
#if USE_TIMECONTROL
    movetime t;
#endif
} moverecord;

extern char* aszGameResult[], szDefaultPrompt[], *szPrompt;

typedef enum {
    GAME_NONE, GAME_PLAYING, GAME_OVER, GAME_RESIGNED, GAME_DROP
#if USE_TIMECONTROL
	, GAME_TIMEOUT
#endif
} gamestate; 

/* The match state is represented by the board position (anBoard),
   fTurn (indicating which player makes the next decision), fMove
   (which indicates which player is on roll: normally the same as
   fTurn, but occasionally different, e.g. if a double has been
   offered).  anDice indicate the roll to be played (0,0 indicates the
   roll has not been made). */
typedef struct {
    int anBoard[ 2 ][ 25 ], anDice[ 2 ], fTurn, fResigned,
	fResignationDeclined, fDoubled, cGames, fMove, fCubeOwner, fCrawford,
	fPostCrawford, nMatchTo, anScore[ 2 ], nCube, cBeavers;
    bgvariation bgv;
    int fCubeUse;
    int fJacoby;
    gamestate gs;
#if USE_TIMECONTROL
    gameclock gc;
    struct timeval tvTimeleft[2];
    int nTimeouts[2];
#endif
} matchstate;

typedef struct _matchinfo { /* SGF match information */
    char* pchRating[ 2 ], *pchEvent, *pchRound, *pchPlace, *pchAnnotator,
	*pchComment; /* malloc()ed, or NULL if unknown */
    int nYear, nMonth, nDay; /* 0 for nYear means date unknown */
} matchinfo;

extern matchstate ms;
extern matchinfo mi;
extern int fNextTurn, fComputing;

/* User settings. */
extern int fAutoGame, fAutoMove, fAutoRoll, fAutoCrawford, cAutoDoubles,
    fCubeUse, fDisplay, fAutoBearoff, fShowProgress,
    nBeavers, fJacoby,
    fOutputRawboard, fAnnotation, cAnalysisMoves, fAnalyseCube,
    fAnalyseDice, fAnalyseMove, fRecord, fMessage, fGameList, nDefaultLength;
extern int fInvertMET;
extern int fConfirm, fConfirmSave;
extern float rAlpha, rAnneal, rThreshold, arLuckLevel[ LUCK_VERYGOOD + 1 ],
    arSkillLevel[ N_SKILLS ], rEvalsPerSec;
extern int nThreadPriority;
extern int fCheat;
extern int afCheatRoll[ 2 ];
extern int fGotoFirstGame;
extern int fSconyers15x15DVD;
extern char szPathSconyers15x15DVD[ BIG_PATH ];      
extern int fSconyers15x15Disk;
extern char szPathSconyers15x15Disk[ BIG_PATH ];      
extern char szLang[ 32 ];   

typedef enum _pathformat {
  PATH_EPS, PATH_GAM, PATH_HTML, PATH_LATEX, PATH_MAT, PATH_OLDMOVES,
  PATH_PDF, PATH_POS, PATH_POSTSCRIPT, PATH_SGF, PATH_SGG, PATH_TEXT, 
  PATH_MET, PATH_TMG, PATH_BKG, PATH_SNOWIE_TXT,
  NUM_PATHS } 
pathformat;

extern char aaszPaths[ NUM_PATHS ][ 2 ][ 255 ];
extern char* aszExtensions[ NUM_PATHS ];
extern char* szCurrentFileName;

extern evalcontext ecTD;
extern evalcontext ecLuck;

extern evalsetup esEvalCube, esEvalChequer;
extern evalsetup esAnalysisCube, esAnalysisChequer;

extern movefilter aamfEval[ MAX_FILTER_PLIES ][ MAX_FILTER_PLIES ];
extern movefilter aamfAnalysis[ MAX_FILTER_PLIES ][ MAX_FILTER_PLIES ];

extern rolloutcontext rcRollout;

extern int fCubeEqualChequer, fPlayersAreSame, fTruncEqualPlayer0;


/* The current match.
  A list of games. Each game is a list of moverecords.
  Note that the first list element is empty. The first game is in
  lMatch.plNext->p. Same is true for games.
*/
extern list lMatch;

/*  List of moverecords representing the current game. One of the elements in
    lMatch.
    Typically the last game in the match).
*/
extern list* plGame;

/* Current move inside plGame (typically the most recently
   one played, but "previous" and "next" commands navigate back and forth).
*/
extern list* plLastMove;

extern statcontext scMatch;

/* There is a global storedmoves struct to maintain the list of moves
   for "=n" notation (e.g. "hint", "rollout =1 =2 =4").

   Anything that _writes_ stored moves ("hint", "show moves", "add move")
   should free the old dynamic move list first (sm.ml.amMoves), if it is
   non-NULL.

   Anything that _reads_ stored moves should check that the move is still
   valid (i.e. auchKey matches the current board and anDice matches the
   current dice). */
typedef struct _storedmoves {
    movelist ml;
    matchstate ms;
} storedmoves;
extern storedmoves sm;

/*
 * Store cube analysis
 *
 */

typedef struct _storedcube {
  float aarOutput[ 2 ][ NUM_ROLLOUT_OUTPUTS ];
  float aarStdDev[ 2 ][ NUM_ROLLOUT_OUTPUTS ];
  evalsetup es;
  matchstate ms;
} storedcube;
extern storedcube sc;


extern player ap[ 2 ];

extern char* GetInput( char* szPrompt );
extern int GetInputYN( char* szPrompt );
extern void HandleCommand( char* sz, command* ac );
extern void InitBoard( int anBoard[ 2 ][ 25 ], const bgvariation bgv );
extern char* NextToken( char **ppch );
extern char* NextTokenGeneral( char **ppch, const char* szTokens );
extern int NextTurn( int fPlayNext );
extern void TurnDone( void );
extern void AddMoveRecord( void* pmr );
extern moverecord* LinkToDouble( moverecord* pmr);
extern void ApplyMoveRecord( matchstate* pms, const list* plGame,
			     const moverecord* pmr );
extern void SetMoveRecord( void* pmr );
extern void ClearMoveRecord( void );
extern void AddGame( moverecord* pmr );
extern void ChangeGame( list* plGameNew );
extern void
FixMatchState ( matchstate* pms, const moverecord* pmr );
extern void CalculateBoard( void );
extern void CancelCubeAction( void );
extern int ComputerTurn( void );
extern void ClearMatch( void );
extern void FreeMatch( void );
extern void SetMatchDate( matchinfo* pmi );
extern int GetMatchStateCubeInfo( cubeinfo* pci, const matchstate* pms);
extern int ParseHighlightColour( char* sz );
extern int ParseNumber( char **ppch );
extern int ParsePlayer( char* sz );
extern int ParsePosition( int an[ 2 ][ 25 ], char **ppch, char* pchDesc );
extern double ParseReal( char **ppch );
extern int ParseKeyValue( char **ppch, char* apch[ 2 ] );
extern int CompareNames( char* sz0, char* sz1 );
extern int SetToggle( char* szName, int* pf, char* sz, char* szOn,
		       char* szOff );
extern void ShowBoard( void );
extern void SetMatchID ( const char* szMatchID );
extern char* FormatPrompt( void );
extern char* FormatMoveHint( char* sz, matchstate* pms, movelist* pml,
			     int i, int fRankKnown,
                             int fDetailProb, int fShowParameters );
extern void UpdateSetting( void* p );
extern void UpdateSettings( void );
extern void ResetInterrupt( void );
extern void PromptForExit( void );
extern void Prompt( void );
extern void PortableSignal( int nSignal, RETSIGTYPE (*p)(int),
			    psighandler* pOld, int fRestart );
extern void PortableSignalRestore( int nSignal, psighandler* p );
extern RETSIGTYPE HandleInterrupt( int idSignal );

/* Like strncpy, except it does the right thing */
extern char* strcpyn( char* szDest, const char* szSrc, int cch );

/* Write a string to stdout/status bar/popup window */
extern void output( const char* sz );
/* Write a string to stdout/status bar/popup window, and append \n */
extern void outputl( const char* sz );
/* Write a character to stdout/status bar/popup window */
extern void outputc( const char ch );
/* Write a string to stdout/status bar/popup window, printf style */
extern void outputf( const char* sz, ... ) __attribute__((format(printf,1,2)));
/* Write a string to stdout/status bar/popup window, vprintf style */
extern void outputv( const char* sz, va_list val )
    __attribute__((format(printf,1,0)));
/* Write an error message, perror() style */
extern void outputerr( const char* sz );
/* Write an error message, fprintf() style */
extern void outputerrf( const char* sz, ... )
    __attribute__((format(printf,1,2)));
/* Write an error message, vfprintf() style */
extern void outputerrv( const char* sz, va_list val )
    __attribute__((format(printf,1,0)));
/* Signifies that all output for the current command is complete */
extern void outputx( void );
/* Temporarily disable outputx() calls */
extern void outputpostpone( void );
/* Re-enable outputx() calls */
extern void outputresume( void );
/* Signifies that subsequent output is for a new command */
extern void outputnew( void );
/* Disable output */
extern void outputoff( void );
/* Enable output */
extern void outputon( void );

/* Temporarily ignore TTY/GUI input. */
extern void SuspendInput( monitor* pm );
/* Resume input (must match a previous SuspendInput). */
extern void ResumeInput( monitor* pm );

extern void ProgressStart( char* sz );
extern void ProgressStartValue( char* sz, int iMax );
extern void Progress( void );
extern void ProgressValue ( int iValue );
extern void ProgressValueAdd ( int iValue );
extern void ProgressEnd( void );

#if USE_GUI
#if USE_GTK
extern gint NextTurnNotify( gpointer p );
#else
extern int NextTurnNotify( event* pev, void* p );
#endif
extern void UserCommand( char* sz );
extern void HandleXAction( void );
#if HAVE_LIBREADLINE
extern int fReadingCommand;
extern void HandleInput( char* sz );
#endif
#endif
#if USE_GUI
#if USE_GTK
extern void HideAllPanels ( gpointer *p, guint n, GtkWidget *pw );
extern void ShowAllPanels ( gpointer *p, guint n, GtkWidget *pw );
#endif
#endif

#if HAVE_LIBREADLINE
extern int fReadline;
#endif

extern int
AnalyzeMove ( moverecord* pmr, matchstate* pms, list* plGame, statcontext* psc,
              evalsetup* pesChequer, evalsetup* pesCube,
              movefilter aamf[ MAX_FILTER_PLIES ][ MAX_FILTER_PLIES ],
	      int fUpdateStatistics, const int afAnalysePlayers[ 2 ] );

extern int
confirmOverwrite ( const char* sz, const int f );

extern void
setDefaultPath ( const char* sz, const pathformat f );

extern void
setDefaultFileName ( const char* sz, const pathformat f );

extern char *
getDefaultFileName ( const pathformat f );

extern char *
getDefaultPath ( const pathformat f );

extern char* GetLuckAnalysis( matchstate* pms, float rLuck );

extern moverecord *
getCurrentMoveRecord ( int* pfHistory );

extern int
getFinalScore( int* anScore );

extern void
UpdateStoredMoves ( const movelist* pml, const matchstate* pms );

extern void
UpdateStoredCube ( float aarOutput[ 2 ][ NUM_ROLLOUT_OUTPUTS ],
                   float aarStdDev[ 2 ][ NUM_ROLLOUT_OUTPUTS ],
                   const evalsetup* pes,
                   const matchstate* pms );

extern void
InvalidateStoredMoves( void );

extern char* aszVersion[], *szHomeDirectory, *szDataDirectory,
    *szTerminalCharset;

extern char* aszSkillType[], *aszSkillTypeAbbr[], *aszLuckType[],
    *aszLuckTypeAbbr[], *aszSkillTypeCommand[], *aszLuckTypeCommand[];

extern command acDatabase[], acNew[], acSave[], acSetAutomatic[],
    acSetCube[], acSetEvaluation[], acSetPlayer[], acSetRNG[], 
    acSetRollout[], acSetRolloutLate[], acSetRolloutLimit[], 
  acSetTruncation [], acSetRolloutJsd[],
    acSet[], acShow[], acTrain[], acTop[], acSetMET[], acSetEvalParam[],
    acSetRolloutPlayer[], acSetRolloutLatePlayer[], cOnOff, cFilename,
    cHighlightColour;
extern command acSetCheatPlayer[];
extern command acSetAnalysisPlayer[];

extern command acAnnotateMove[];
extern command acSetExportParameters[];
extern command acSetGeometryValues[];
extern command acSetHighlightIntensity[];

extern float rRatingOffset;


extern int
InternalCommandNext(int fMarkedMoves, int n);

extern void CommandAccept( char * ),
    CommandAgree( char * ),
    CommandAnalyseClearGame( char * ),
    CommandAnalyseClearMatch( char * ),
    CommandAnalyseClearMove( char * ),
    CommandAnalyseGame( char * ),
    CommandAnalyseMatch( char * ),
    CommandAnalyseMove( char * ),
    CommandAnalyseSession( char * ),
    CommandAnnotateAccept ( char * ),
    CommandAnnotateBad( char * ),
    CommandAnnotateClearComment( char * ),
    CommandAnnotateClearLuck( char * ),
    CommandAnnotateClearSkill( char * ),
    CommandAnnotateCube ( char * ),
    CommandAnnotateDouble ( char * ),
    CommandAnnotateDoubtful( char * ),
    CommandAnnotateDrop ( char * ),
    CommandAnnotateGood( char * ),
    CommandAnnotateInteresting( char * ),
    CommandAnnotateLucky( char * ),
    CommandAnnotateMove ( char * ),
    CommandAnnotateReject ( char * ),
    CommandAnnotateResign ( char * ),
    CommandAnnotateUnlucky( char * ),
    CommandAnnotateVeryBad( char * ),
    CommandAnnotateVeryGood( char * ),
    CommandAnnotateVeryLucky( char * ),
    CommandAnnotateVeryUnlucky( char * ),
    CommandCalibrate( char* sz ),
    CommandCopy ( char * ),
    CommandDatabaseDump( char * ),
    CommandDatabaseExport( char * ),
    CommandDatabaseImport( char * ),
    CommandDatabaseRollout( char * ),
    CommandDatabaseGenerate( char * ),
    CommandDatabaseTrain( char * ),
    CommandDatabaseVerify( char * ),
    CommandDecline( char * ),
    CommandDiceRolls( char * ),
    CommandDouble( char * ),
    CommandDrop( char * ),
    CommandEval( char * ),
    CommandEq2MWC( char * ),
    CommandExportGameGam( char * ),
    CommandExportGameHtml( char * ),
    CommandExportGameLaTeX( char * ),
    CommandExportGamePDF( char * ),
    CommandExportGamePostScript( char * ),
    CommandExportGameText( char * ),
    CommandExportGameEquityEvolution ( char * ),
    CommandExportHTMLImages ( char * ),
    CommandExportMatchMat( char * ),
    CommandExportMatchHtml( char * ),
    CommandExportMatchLaTeX( char * ),
    CommandExportMatchPDF( char * ),
    CommandExportMatchPostScript( char * ),
    CommandExportMatchText( char * ),
    CommandExportMatchEquityEvolution ( char * ),
    CommandExportMatchSGG( char * ),
    CommandExportMatchTMG( char * ),
    CommandExportPositionEPS( char * ),
    CommandExportPositionPNG( char * ),
    CommandExportPositionJF( char * ),
    CommandExportPositionHtml( char * ),
    CommandExportPositionText( char * ),
    CommandExportPositionGammOnLine ( char * ),
    CommandExportPositionGOL2Clipboard ( char * ),
    CommandExportPositionSnowieTxt( char * ),
    CommandExternal( char * ),
    CommandFirstGame( char * ),
    CommandHelp( char * ),
    CommandHint( char * ),
    CommandImportBKG( char * ),
    CommandImportJF( char * ),
    CommandImportMat( char * ),
    CommandImportOldmoves( char * ),
    CommandImportSGG( char * ),
    CommandImportSnowieTxt( char * ),
    CommandImportTMG( char * ),
    CommandListGame( char * ),
    CommandListMatch( char * ),
    CommandLoadCommands( char * ),
    CommandLoadGame( char * ),
    CommandLoadMatch( char * ),
    CommandLoadPosition( char * ),
    CommandMove( char * ),
    CommandMWC2Eq( char * ),
    CommandNewGame( char * ),
    CommandNewMatch( char * ),
    CommandNewSession( char * ),
    CommandNewWeights( char * ),
    CommandNext( char * ),
    CommandNotImplemented( char * ),
    CommandPlay( char * ),
    CommandPrevious( char * ),
    CommandQuit( char * ),
    CommandRedouble( char * ),
    CommandReject( char * ),
    CommandRecordAddGame( char * ),
    CommandRecordAddMatch( char * ),
    CommandRecordAddSession( char * ),
    CommandRecordErase( char * ),
    CommandRecordEraseAll( char * ),
    CommandRecordShow( char * ),
    CommandResign( char * ),
    CommandRoll( char * ),
    CommandRollout( char * ),
    CommandSaveGame( char * ),
    CommandSaveMatch( char * ),
    CommandSavePosition( char * ),
    CommandSaveSettings( char * ),
    CommandSaveWeights( char * ),
    CommandSetAnalysisChequerplay( char * ),
    CommandSetAnalysisCube( char * ),
    CommandSetAnalysisCubedecision( char * ),
    CommandSetAnalysisLimit( char * ),
    CommandSetAnalysisLuckAnalysis( char * ),
    CommandSetAnalysisLuck( char * ),
    CommandSetAnalysisMoveFilter( char * ),
    CommandSetAnalysisMoves( char * ),
    CommandSetAnalysisPlayer( char * ),
    CommandSetAnalysisPlayerAnalyse( char * ),
    CommandSetAnalysisThresholdBad( char * ),
    CommandSetAnalysisThresholdDoubtful( char * ),
    CommandSetAnalysisThresholdGood( char * ),
    CommandSetAnalysisThresholdInteresting( char * ),
    CommandSetAnalysisThresholdLucky( char * ),
    CommandSetAnalysisThresholdUnlucky( char * ),
    CommandSetAnalysisThresholdVeryBad( char * ),
    CommandSetAnalysisThresholdVeryGood( char * ),
    CommandSetAnalysisThresholdVeryLucky( char * ),
    CommandSetAnalysisThresholdVeryUnlucky( char * ),
    CommandSetAnnotation( char * ),
    CommandSetAppearance( char * ),
    CommandSetAutoAnalysis( char * ),
    CommandSetAutoBearoff( char * ),
    CommandSetAutoCrawford( char * ),
    CommandSetAutoDoubles( char * ),
    CommandSetAutoGame( char * ),
    CommandSetAutoMove( char * ),
    CommandSetAutoRoll( char * ),
    CommandSetBearoffSconyers15x15DVDEnable( char * ),
    CommandSetBearoffSconyers15x15DVDPath( char * ),
    CommandSetBearoffSconyers15x15DiskEnable( char * ),
    CommandSetBearoffSconyers15x15DiskPath( char * ),
    CommandSetBoard( char * ),
    CommandSetBeavers( char * ),
    CommandSetCache( char * ),
    CommandSetCalibration( char * ),
    CommandSetCheatEnable ( char * ),
    CommandSetCheatPlayer ( char * ),
    CommandSetCheatPlayerRoll ( char * ),
    CommandSetClockwise( char * ),
    CommandSetConfirmNew( char * ),
    CommandSetConfirmSave( char * ),
    CommandSetCrawford( char * ),
    CommandSetCubeCentre( char * ),
    CommandSetCubeOwner( char * ),
    CommandSetCubeUse( char * ),
    CommandSetCubeValue( char * ),
    CommandSetCubeEfficiencyOS( char * ),
    CommandSetCubeEfficiencyRaceFactor( char * ),
    CommandSetCubeEfficiencyRaceCoefficient( char * ),
    CommandSetCubeEfficiencyRaceMax( char * ),
    CommandSetCubeEfficiencyRaceMin( char * ),
    CommandSetCubeEfficiencyCrashed( char * ),
    CommandSetCubeEfficiencyContact( char * ),
    CommandSetDelay( char * ),
    CommandSetDice( char * ),
    CommandSetDisplay( char * ),
    CommandSetDisplayPanels( char *),
    CommandSetEvalCandidates( char * ),
    CommandSetEvalCubeful( char * ),
    CommandSetEvalDeterministic( char * ),
    CommandSetEvalNoOnePlyPrune( char * ),
    CommandSetEvalNoise( char * ),    
    CommandSetEvalPlies( char * ),
    CommandSetEvalReduced ( char * ),
    CommandSetEvalTolerance( char * ),
    CommandSetEvaluation( char * ),
    CommandSetEvalParamType( char * ),
    CommandSetEvalParamRollout( char * ),
    CommandSetEvalParamEvaluation( char * ),
    CommandSetEvalChequerplay ( char * ),
    CommandSetEvalCubedecision ( char * ),
    CommandSetEvalMoveFilter ( char * ),
    CommandSetEgyptian( char * ),
    CommandSetExportIncludeAnnotations ( char * ),
    CommandSetExportIncludeAnalysis ( char * ),
    CommandSetExportIncludeStatistics ( char * ),
    CommandSetExportIncludeLegend ( char * ),
    CommandSetExportIncludeMatchInfo ( char * ),
    CommandSetExportShowBoard ( char * ),
    CommandSetExportShowPlayer ( char * ),
    CommandSetExportMovesNumber ( char * ),
    CommandSetExportMovesProb ( char * ),
    CommandSetExportMovesParameters ( char * ),
    CommandSetExportMovesDisplayVeryBad ( char * ),
    CommandSetExportMovesDisplayBad ( char * ),
    CommandSetExportMovesDisplayDoubtful ( char * ),
    CommandSetExportMovesDisplayUnmarked ( char * ),
    CommandSetExportMovesDisplayInteresting ( char * ),
    CommandSetExportMovesDisplayGood ( char * ),
    CommandSetExportMovesDisplayVeryGood ( char * ),
    CommandSetExportCubeProb ( char * ),
    CommandSetExportCubeParameters ( char * ),
    CommandSetExportCubeDisplayVeryBad ( char * ),
    CommandSetExportCubeDisplayBad ( char * ),
    CommandSetExportCubeDisplayDoubtful ( char * ),
    CommandSetExportCubeDisplayUnmarked ( char * ),
    CommandSetExportCubeDisplayInteresting ( char * ),
    CommandSetExportCubeDisplayGood ( char * ),
    CommandSetExportCubeDisplayVeryGood ( char * ),
    CommandSetExportCubeDisplayActual ( char * ),
    CommandSetExportCubeDisplayMissed ( char * ),    
    CommandSetExportCubeDisplayClose ( char * ),
    CommandSetExportHTMLCSSExternal ( char * ),
    CommandSetExportHTMLCSSHead ( char * ),
    CommandSetExportHTMLCSSInline ( char * ),
    CommandSetExportHTMLPictureURL ( char * ),
    CommandSetExportHTMLTypeBBS ( char * ),
    CommandSetExportHTMLTypeFibs2html ( char * ),
    CommandSetExportHTMLTypeGNU ( char * ),
    CommandSetExportParametersEvaluation ( char * ),
    CommandSetExportParametersRollout ( char * ),
    CommandSetExportPNGSize ( char *),
    CommandSetGeometryAnnotation ( char * ),
    CommandSetGeometryGame ( char * ),
    CommandSetGeometryHint ( char * ),
    CommandSetGeometryMain ( char * ),
    CommandSetGeometryMessage ( char * ),
    CommandSetGeometryWidth ( char * ),
    CommandSetGeometryHeight ( char * ),
    CommandSetGeometryPosX ( char * ),
    CommandSetGeometryPosY ( char * ),
    CommandSetGotoFirstGame( char * ),
    CommandSetGUIAnimationBlink( char * ),
    CommandSetGUIAnimationNone( char * ),
    CommandSetGUIAnimationSlide( char * ),
    CommandSetGUIAnimationSpeed( char * ),
    CommandSetGUIBeep( char * ),
    CommandSetGUIDiceArea( char * ),
    CommandSetGUIHighDieFirst( char * ),
    CommandSetGUIIllegal( char * ),
    CommandSetGUIWindowPositions( char * ),
    CommandSetGUIShowIDs( char * ),
    CommandSetGUIShowPips( char * ),
    CommandSetGUIDragTargetHelp( char * ),
    CommandSetGUIUseStatsPanel( char * ),
    CommandSetHighlight ( char * ),
    CommandSetHighlightColour ( char * ),
    CommandSetHighlightDark ( char * ),
    CommandSetHighlightLight ( char * ),
    CommandSetHighlightMedium ( char * ),
    CommandSetInvertMatchEquityTable( char * ),
    CommandSetJacoby( char * ),
    CommandSetLang( char * ),
    CommandSetMatchAnnotator( char * ),
    CommandSetMatchComment( char * ),
    CommandSetMatchDate( char * ),
    CommandSetMatchEvent( char * ),
    CommandSetMatchLength( char * ),
    CommandSetMatchPlace( char * ),
    CommandSetMatchRating( char * ),
    CommandSetMatchRound( char * ),
    CommandSetMatchID ( char * ),
    CommandSetMessage ( char * ),
    CommandSetMET( char * ),
    CommandSetMoveFilter( char * ),
    CommandSetOutputDigits( char *),
    CommandSetOutputMatchPC( char * ),
    CommandSetOutputMWC ( char * ),
    CommandSetOutputRawboard( char * ),
    CommandSetOutputWinPC( char * ),
    CommandSetOutputErrorRateFactor( char * ),
    CommandSetPathEPS( char * ),
    CommandSetPathSGF( char * ),
    CommandSetPathLaTeX( char * ),
    CommandSetPathPDF( char * ),
    CommandSetPathHTML( char * ),
    CommandSetPathMat( char * ),
    CommandSetPathMET( char * ),
    CommandSetPathSGG( char * ),
    CommandSetPathTMG( char * ),
    CommandSetPathOldMoves( char * ),
    CommandSetPathPos( char * ),
    CommandSetPathGam( char * ),
    CommandSetPathPostScript( char * ),
    CommandSetPathSnowieTxt( char * ),
    CommandSetPathText( char * ),
    CommandSetPathBKG( char * ),
    CommandSetPlayerChequerplay( char * ),
    CommandSetPlayerCubedecision( char * ),
    CommandSetPlayerExternal( char * ),
    CommandSetPlayerGNU( char * ),
    CommandSetPlayerHuman( char * ),
    CommandSetPlayerName( char * ),
    CommandSetPlayerMoveFilter( char * ),
    CommandSetPlayerPlies( char * ),
    CommandSetPlayerPubeval( char * ),
    CommandSetPlayer( char * ),
    CommandSetPostCrawford( char * ),
    CommandSetPriorityAboveNormal ( char * ),
    CommandSetPriorityBelowNormal ( char * ),
    CommandSetPriorityHighest ( char * ),
    CommandSetPriorityIdle ( char * ),
    CommandSetPriorityNice( char * ),
    CommandSetPriorityNormal ( char * ),
    CommandSetPriorityTimeCritical ( char * ),
    CommandSetPrompt( char * ),
    CommandSetRatingOffset( char * ),
    CommandSetRecord( char * ),
    CommandSetRNG( char * ),
    CommandSetRNGAnsi( char * ),
    CommandSetRNGBBS( char * ),
    CommandSetRNGBsd( char * ),
    CommandSetRNGFile( char * ),
    CommandSetRNGIsaac( char * ),
    CommandSetRNGManual( char * ),
    CommandSetRNGMD5( char * ),
    CommandSetRNGMersenne( char * ),
    CommandSetRNGRandomDotOrg( char * ),
    CommandSetRNGUser( char * ),
    CommandSetRollout ( char * ),
    CommandSetRolloutBearoffTruncationExact ( char * ),
    CommandSetRolloutBearoffTruncationOS ( char * ),
    CommandSetRolloutCubedecision ( char * ),
    CommandSetRolloutLateCubedecision ( char * ),
    CommandSetRolloutCubeful ( char * ),
    CommandSetRolloutChequerplay ( char * ),
    CommandSetRolloutCubeEqualChequer ( char * ),
    CommandSetRolloutInitial( char * ),
    CommandSetRolloutJsd( char * ),
    CommandSetRolloutJsdEnable( char * ),
    CommandSetRolloutJsdLimit( char * ),
    CommandSetRolloutJsdMinGames( char * ),
    CommandSetRolloutJsdMoveEnable( char * ),
    CommandSetRolloutLimit( char * ),
    CommandSetRolloutLimitEnable( char * ),
    CommandSetRolloutLimitMinGames( char * ),
    CommandSetRolloutMaxError( char * ),
    CommandSetRolloutMoveFilter( char * ),
    CommandSetRolloutPlayer ( char * ),
    CommandSetRolloutPlayerChequerplay ( char * ),
    CommandSetRolloutPlayerCubedecision ( char * ),
    CommandSetRolloutPlayerMoveFilter( char * ),
    CommandSetRolloutPlayerLateChequerplay ( char * ),
    CommandSetRolloutPlayerLateCubedecision ( char * ),
    CommandSetRolloutPlayerLateMoveFilter( char * ),
    CommandSetRolloutPlayersAreSame( char * ),
    CommandSetRolloutLate ( char * ),
    CommandSetRolloutLateChequerplay ( char * ),
    CommandSetRolloutLateMoveFilter( char * ),
    CommandSetRolloutLateEnable ( char * ),
    CommandSetRolloutLatePlayer ( char * ),
    CommandSetRolloutLatePlies ( char * ),
    CommandSetRolloutTruncationChequer ( char * ),
    CommandSetRolloutTruncationCube ( char * ),
    CommandSetRolloutTruncationEnable ( char * ),
    CommandSetRolloutTruncationEqualPlayer0 (char *),
    CommandSetRolloutTruncationPlies ( char * ),    
    CommandSetRolloutRNG ( char * ),
    CommandSetRolloutRotate ( char * ),
    CommandSetRolloutSeed( char * ),
    CommandSetRolloutTrials( char * ),
    CommandSetRolloutTruncation( char * ),
    CommandSetRolloutVarRedn( char * ),
    CommandSetScore( char * ),
    CommandSetSeed( char * ),
    CommandSetSoundEnable ( char * ),
    CommandSetSoundSystemArtsc ( char * ),
    CommandSetSoundSystemCommand ( char * ),
    CommandSetSoundSystemESD ( char * ),
    CommandSetSoundSystemNAS ( char * ),
    CommandSetSoundSystemNormal ( char * ),
    CommandSetSoundSystemWindows ( char * ),
    CommandSetSoundSystemQuickTime ( char * ),
    CommandSetSoundSoundAgree ( char * ),
    CommandSetSoundSoundAnalysisFinished ( char * ),
    CommandSetSoundSoundBotDance ( char * ),
    CommandSetSoundSoundBotWinGame ( char * ),
    CommandSetSoundSoundBotWinMatch ( char * ),
    CommandSetSoundSoundDouble ( char * ),
    CommandSetSoundSoundDrop ( char * ),
    CommandSetSoundSoundExit ( char * ),
    CommandSetSoundSoundHumanDance ( char * ),
    CommandSetSoundSoundHumanWinGame ( char * ),
    CommandSetSoundSoundHumanWinMatch ( char * ),
    CommandSetSoundSoundMove ( char * ),
    CommandSetSoundSoundRedouble ( char * ),
    CommandSetSoundSoundResign ( char * ),
    CommandSetSoundSoundRoll ( char * ),
    CommandSetSoundSoundStart ( char * ),
    CommandSetSoundSoundTake ( char * ),
#if USE_TIMECONTROL
    CommandSetTCMove( char * ),
    CommandSetTCMultiplier( char * ),
    CommandSetTCName( char * ),
    CommandSetTCNext( char * ),
    CommandSetTCPenalty( char * ),
    CommandSetTCPoint( char * ),
    CommandSetTCTime( char * ),
    CommandSetTCType( char * ),
    CommandSetTCUnname( char * ),
    CommandSetTimeControl( char * ),
#endif
    CommandSetTrainingAlpha( char * ),
    CommandSetTrainingAnneal( char * ),
    CommandSetTrainingThreshold( char * ),
    CommandSetTurn( char * ),
    CommandSetTutorCube( char * ),
    CommandSetTutorEval( char* sz ),
    CommandSetTutorChequer( char * ),
    CommandSetTutorMode( char * ),  
    CommandSetTutorSkillDoubtful( char* sz ),
    CommandSetTutorSkillBad( char* sz ), 
    CommandSetTutorSkillVeryBad( char* sz ),
    CommandSetVariationStandard( char * sz ),
    CommandSetVariationNackgammon( char * sz ),
    CommandSetVariation1ChequerHypergammon( char * sz ),
    CommandSetVariation2ChequerHypergammon( char * sz ),
    CommandSetVariation3ChequerHypergammon( char * sz ),
    CommandShowAnalysis( char * ),
    CommandShowAutomatic( char * ),
    CommandShowBoard( char * ),
    CommandShowBeavers( char * ),
    CommandShowCache( char * ),
    CommandShowCalibration( char * ),
    CommandShowCheat( char * ),
    CommandShowClockwise( char * ),
    CommandShowCommands( char * ),
    CommandShowConfirm( char * ),
    CommandShowCopying( char * ),
    CommandShowCrawford( char * ),
    CommandShowCube( char * ),
    CommandShowCubeEfficiency( char * ),
    CommandShowDelay( char * ),
    CommandShowDice( char * ),
    CommandShowDisplay( char * ),
    CommandShowDisplayPanels( char * ),
    CommandShowEngine( char * ),
    CommandShowEvaluation( char * ),
    CommandShowExport ( char * ),
    CommandShowFullBoard( char * ),
    CommandShowGammonValues( char * ),
    CommandShowGeometry ( char * ),
    CommandShowHighlightColour ( char * ),
    CommandShowEgyptian( char * ),
    CommandShowJacoby( char * ),
    CommandShowKleinman( char * ),
    CommandShowLang( char * ),
    CommandShowMarketWindow( char * ),
    CommandShowMatchInfo( char * ),
    CommandShowMatchLength( char * ),
    CommandShowMatchEquityTable( char * ),
    CommandShowMatchResult( char * ),
    CommandShowOneChequer ( char * ),
    CommandShowOneSidedRollout ( char * ),
    CommandShowOutput( char * ),
    CommandShowPath( char * ),
    CommandShowPipCount( char * ),
    CommandShowPostCrawford( char * ),
    CommandShowPlayer( char * ),
    CommandShowPrompt( char * ),
    CommandShowRNG( char * ),
    CommandShowRollout( char * ),
    CommandShowRolls ( char * ),
    CommandShowScore( char * ),
    CommandShowBearoff( char * ),
    CommandShowSeed( char * ),
    CommandShowSound( char * ),
    CommandShowStatisticsGame( char * ),
    CommandShowStatisticsMatch( char * ),
    CommandShowStatisticsSession( char * ),
#if USE_TIMECONTROL
    CommandShowTCList( char * ),
    CommandShowTCTutorial( char * ),
    CommandShowTimeControl( char * ),
#endif
    CommandShowTemperatureMap( char * ),
    CommandShowThorp( char * ),
    CommandShowTraining( char * ),
    CommandShowTurn( char * ),
    CommandShowTutor( char * ), 
    CommandShowVariation( char * ),
    CommandShowVersion( char * ),
    CommandShowWarranty( char * ),
    CommandSwapPlayers ( char * ),
    CommandTake( char * ),
    CommandTrainTD( char * ),
    CommandXCopy ( char * );


extern int fTutor, fTutorCube, fTutorChequer, nTutorSkillCurrent;

extern int fDisplayPanels;

extern int GiveAdvice ( skilltype Skill );
extern skilltype TutorSkill;
extern int fTutorAnalysis;

extern int EvalCmp ( const evalcontext *, const evalcontext *, const int);

#ifndef HAVE_BASENAME
extern char *
basename ( const char* filename );
#endif

#ifndef HAVE_DIRNAME
extern char *
dirname ( char* filename );
#endif

#if USE_GTK
#  if GTK_CHECK_VERSION(1,3,0) || defined (WIN32)
#    define GNUBG_CHARSET "UTF-8"
#  else
#    define GNUBG_CHARSET "ISO-8859-1"
#  endif
#else
#  define GNUBG_CHARSET "ISO-8859-1"
#endif

extern char *
Convert ( const char* sz, 
          const char* szSourceCharset, const char* szDestCharset );

extern void
OptimumRoll ( int anBoard[ 2 ][ 25 ], 
              const cubeinfo* pci, const evalcontext* pec,
              const int fBest, int anDice[ 2 ] );

typedef struct _highlightcolour {
  int   rgbs[3][3];
  char* colourname;
} highlightcolour;

extern highlightcolour* HighlightColour, HighlightColourTable[];
extern int HighlightIntensity;
extern int* Highlightrgb;

extern void
SetMatchInfo( char **ppch, char* sz, char* szMessage );

extern void
TextToClipboard ( const char * sz );

extern void
PrintCheatRoll( const int fPlayer, const int n );

extern void
ShowBearoff( char* sz, matchstate* pms, bearoffcontext* pbc );

#endif
