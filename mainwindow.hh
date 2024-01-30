/* COMP.CS.110 - Project IV: Path GUI
 *
 * Program author: Andrea Caruso
 * Student number: 151211751
 * UserID: ccanca
 * Email: andrea.caruso@tuni.fi
 *
 * Description:
 * This class implements a simple GUI for the Path (Polku) board game; whose
 * logic is implemented in the GameBoard class. This clas has minimal
 * connection with the logic part of the game, interacting with the
 * BoardGame class only using the BoardGame class' public methods which are:
 *
 *     ...->move(start, destination)
 *     ...->is_game_over(point)
 *     ...->which_slot(point)
 *     ...->is_valid_point()
 *
 * As such, this class is mainly responsible to change the GUI elements to go
 * along with the logic of the game.
 *
 * The GUI has the following features and their respective functionalities.
 *
 *     - Quit: The quit button can be clicked at any time in order to
 *             immediately exit the game and close the window.
 *
 *     - Reset: The reset button can be clicked at any time except right after
 *              clicking it a first time, that is, the game must be either
 *              ongoing or have ended. By clicking the reset button the game
 *              is reset.
 *
 *     - Pause: The toggle pause button can be clicked during an ongoing match
 *              and it lets the player take a break by pausing the timer. In
 *              addition, the button greys out the board so the player can't
 *              pause to reflect on their next move. This button functions as
 *              a toggle.
 *
 *     - High contrast: The toggle high contrast button gives the game palette
 *                      a high contrast makeover in case the player needs some
 *                      aid in recognising the colours. The button can be
 *                      pressed at any time and doesn't affect the game's
 *                      state or functionality (except that it resets the
 *                      player's first click of the move if one was executed).
 *                      The button functions as a toggle.
 *
 * In addition to buttons, this class implements some additional game statistic
 * attributes to reward the player, which are:
 *
 *     - Points: Each game starts with zero points. For each invalid move, the
 *               player loses 3 points. For each valid move, the player is
 *               rewarded 12 points IF the total moves are less than or equal
 *               to 31; otherwise the player loses 6 points for each valid
 *               move. Once the player reaches a victory, the time taken is
 *               subtracted from the total points. After all the calculations,
 *               if the player has reached the victory in exactly 31 moves, 31
 *               points are added to the player's point total. IF the player at
 *               this point has less than zero points, the final score is
 *               simply zero points.
 *
 *     - Moves: Each game starts with zero moves. Each valid move adds one move
 *              to the total moves taken towardsvictory. IF the player wins the
 *              game in exactly 31 moves, the background changes colour to a
 *              golden hue until the game is reset. NOTE! Although invalid
 *              moves reduce points, they are not added to the move total.
 *
 *     - Time: Each game starts with zero seconds on the clock. The time taken
 *             to reach a victory is measured from the first click which is
 *             done on a board tile. The time is measured in seconds. The time
 *             stops temporarily when the game is paused. The time is visualised
 *             using a timer widget.
 *
 * To help with visualising the move, upon the starting point click on the
 * board, the starting block's colour is dimmed to keep track of what block is
 * the starting block. After the starting click for a move, another block is
 * selected as the destination location for the move.  Colours change also
 * using the high contrast toggle button as well as when the player wins with a
 * perfect 31 moves, in which case the GUI window's background becomes golden
 * until the start of the next game.
 *
 * The GUI includes an information board, which gives valuable information
 * about the state of the game. The info board begins each game by displaying
 * some simple rules of how to play. As soon as the game starts, the board
 * informs the player whether the destination is vacant or whether the path is
 * blocked in this order with two respective error messages. Additionally,the
 * board informs the player that starting a move on an empty tile is not
 * allowed. Upon victory, the info board displays the gathered game statistics
 * for the current game round.
 *
 * The game's runtime is kept using a timer which begins as the first starting
 * click is executed and ends as the game is won. The timer only stops during
 * a game if the game is paused. The timer resets with each starting game.
 *
 * */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "gameboard.hh"
#include <utility>
#include <vector>

#include <QMainWindow>
#include <QPushButton>
#include <QTextBrowser>
#include <QLabel>
#include <QTimer>
#include <QLCDNumber>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void setup_gui_board();
    void board_click_handler();
    void toggle_high_contrast();
    void toggle_pause();
    void reset_game();
    void run_time();
    void quit();

private:

    void celebrate_victory();

    // Executive.
    Ui::MainWindow* ui;
    GameBoard gameboard_;

    // Widgets.
    QTimer* tickTock_ = new QTimer(this);
    QTextBrowser* magicBackground_ = new QTextBrowser(this);
    QLCDNumber* timerScreen_ = new QLCDNumber(this);
    QTextBrowser* infoBoard_ = new QTextBrowser(this);

    // Attributes.
    std::pair<Point, Point> start_end_;
    std::vector<std::vector<QPushButton*>> button_grid_;
    unsigned int moves_;
    int points_;
    bool paused_;
    bool in_game_;
    bool high_contrast_;
    bool basking_in_victory_;

    // Reference coordinate.
    const Point null_coordinate = {-1, -1};

    // Colours.
    QString RED_        = "background-color:red";
    QString GREEN_      = "background-color:lightGreen";
    QString PRESSRED_   = "background-color:darkRed";
    QString PRESSGREEN_ = "background-color:green";
    QString EMPTY_      = "background-color:lightGray";
    QString BACKGROUND_ = "background-color:lightGray";
    QString PAUSED_     = "background-color:darkGray";
    QString TIMER_      = "background-color:black";
    QString INFO_       = "background-color:white";

};
#endif // MAINWINDOW_HH
