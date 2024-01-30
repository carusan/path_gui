/* COMP.CS.110 - Project IV: Path GUI
 *
 * Program author: Andrea Caruso
 * Student number: 151211751
 * UserID: ccanca
 * Email: andrea.caruso@tuni.fi
 *
 * Description:
 * This file implements the workings of a simple GUI for the Path (Polku) board
 * game. The details of this file's implemented features are explained in this
 * class' header file 'mainwindow.hh'.
 *
 * */

#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(560, 430));
    this->setWindowTitle("Polku");
    this->setup_gui_board();

    // Setting up GUI elements (NOT the board).
    QPushButton* pauseButton = new QPushButton("Pause", this);
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(toggle_pause()));
    pauseButton->setGeometry(355, 5, 60, 30);
    pauseButton->setStyleSheet("background-color:blue");

    QPushButton* resetButton = new QPushButton("Reset", this);
    connect(resetButton, SIGNAL(clicked()), this, SLOT(setup_gui_board()));
    resetButton->setGeometry(425, 5, 60, 30);
    resetButton->setStyleSheet("background-color:blue");

    QPushButton* quitButton = new QPushButton("Quit", this);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
    quitButton->setGeometry(495, 5, 60, 30);
    quitButton->setStyleSheet("background-color:blue");

    QPushButton* contrastButton = new QPushButton("High contrast", this);
    connect(contrastButton, SIGNAL(clicked()), this, SLOT(toggle_high_contrast()));
    contrastButton->setGeometry(145, 5, 130, 30);
    contrastButton->setStyleSheet("background-color:blue");

    // These elements are defined in the mainwindow.hh file.
    infoBoard_->setGeometry(355, 85, 200, 340);
    timerScreen_->setStyleSheet(INFO_);

    timerScreen_->setGeometry(5, 5, 130, 30);
    timerScreen_->setStyleSheet(TIMER_);

    magicBackground_->setGeometry(0, 0, 560, 470);
    magicBackground_->setStyleSheet(BACKGROUND_);

    connect(tickTock_, SIGNAL(timeout()), this, SLOT(run_time()));

    // Creating the grid row and column label widgets.
    for(unsigned int i = 1; i <= 5; ++i)
    {
        QString Qi = QString::number(i);

        QLabel* rowLabel = new QLabel(Qi, this);
        rowLabel->setGeometry(45, 15 + i * 70, 10, 60);
    }

    for(unsigned int j = 1; j <= 4; ++j)
    {
        QString Qj = QString::number(j);

        QLabel* columnLabel = new QLabel(Qj, this);
        columnLabel->setGeometry(30 + j * 70, 55, 30, 10);
    }

    // Defining all the initial attribute values.
    moves_ = 0;
    points_ = 0;
    paused_ = false;
    in_game_ = false;
    high_contrast_ = false;
    start_end_ = {null_coordinate, null_coordinate};
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup_gui_board()
{
    reset_game();

    // Creating the game tile grid and a way to index them.
    for(unsigned int x = 1; x <= COLUMNS; ++x)
    {
        std::vector<QPushButton*> button_column;
        
        for(unsigned int y = 1; y <= ROWS; ++y)
        {
            QPushButton* pushTile = new QPushButton(this);
            button_column.push_back(pushTile);

            if(((x == 1 or x == 4) and (y == 2 or y == 3 or y == 4))
               or (x == 3 and (y == 2 or y == 4)))
            {
                // These tiles are dummy tiles; deactivated and hidden.
                QSizePolicy empty_policy = pushTile->sizePolicy();
                empty_policy.setRetainSizeWhenHidden(true);
                pushTile->setSizePolicy(empty_policy);

                pushTile->hide();
                pushTile->setDisabled(true);
            }
            else
            {
                connect(pushTile, SIGNAL(clicked()), this, SLOT(board_click_handler()));
            }
            pushTile->setGeometry(5 + x * 70, 15 + y * 70, 60, 60);
        }
        button_grid_.push_back(button_column);
    }

    // Preparing the initial tile positions.
    for(unsigned int x = 0; x < COLUMNS; ++x)
    {
        button_grid_.at(x).at(0)->setStyleSheet(GREEN_);
        button_grid_.at(x).at(1)->setStyleSheet(EMPTY_);
        button_grid_.at(x).at(2)->setStyleSheet(EMPTY_);
        button_grid_.at(x).at(3)->setStyleSheet(EMPTY_);
        button_grid_.at(x).at(4)->setStyleSheet(RED_  );
    }

    for(unsigned int i = 0; i < COLUMNS; ++i)
    {
        for(unsigned int j = 0; j < ROWS; ++j)
        {
            // Needed conversion.
            int signed_x = (int) i;
            int signed_y = (int) j;

            Point x_and_y = {signed_x, signed_y};

            if(gameboard_.which_slot(x_and_y) != UNUSED)
            {
                button_grid_.at(i).at(j)->setDisabled(false);
            }
        }
    }
}

void MainWindow::board_click_handler()
{
    if(not in_game_)
    {
        in_game_ = true;

        tickTock_->start(1000);
    }
    infoBoard_->setText("Pick your next move...");

    int x = null_coordinate.x;
    int y = null_coordinate.y;

    // Figuring out which button was clicked.
    for(unsigned int i = 0; i < COLUMNS; ++i)
    {
        for(unsigned int j = 0; j < ROWS; ++j)
        {
            if(button_grid_.at(i).at(j) == sender())
            {
                x = i;
                y = j;
            }
        }
    }
    Point coords = {x, y};

    if(start_end_.first == null_coordinate)
    {
        if(gameboard_.which_slot(coords) == EMPTY)
        {
            start_end_.first = null_coordinate;
            start_end_.second = null_coordinate;

            infoBoard_->setText("You can only select coloured blocks "
                                "as starting positions."              );
            points_ -= 3;
        }
        else
        {
            start_end_.first = coords;

            QString select_first_colour;

            if(gameboard_.which_slot(coords) == RED)
            {
                select_first_colour = PRESSRED_;
            }
            else if(gameboard_.which_slot(coords) == GREEN)
            {
                select_first_colour = PRESSGREEN_;
            }

            button_grid_.at(x).at(y)->setStyleSheet(select_first_colour);
        }
        return;
    }
    start_end_.second = coords;

    const Point start = start_end_.first;
    const Point destination = start_end_.second;

    QString start_colour;

    if(gameboard_.which_slot(start) == RED)
    {
        start_colour = RED_;
    }
    else if(gameboard_.which_slot(start) == GREEN)
    {
        start_colour = GREEN_;
    }

    int start_type = gameboard_.which_slot(start);
    int destination_type = gameboard_.which_slot(destination);

    if((start_type == RED   and destination_type == GREEN) or
       (start_type == GREEN and destination_type == RED  ) or
       (start_type == destination_type)                      )
    {
        infoBoard_->setText("Your selected destination is not vacant.");
        points_ -= 3;
    }
    else if(gameboard_.move(start, destination))
    {
        int x_s = start.x;
        int y_s = start.y;
        int x_d = destination.x;
        int y_d = destination.y;

        button_grid_.at(x_d).at(y_d)->setStyleSheet(start_colour);
        button_grid_.at(x_s).at(y_s)->setStyleSheet(EMPTY_);

        moves_ += 1;

        if(moves_ <= 31)
        {
            points_ += 12;
        }
        else if(moves_ > 31)
        {
            points_ -= 6;
        }
    }
    else
    {
        infoBoard_->setText("Your selected destination "
                            "is unaccessible to this block.");
        points_ -= 3;
    }
    start_end_.first = null_coordinate;
    start_end_.second = null_coordinate;

    QString revert_colour;

    if(gameboard_.which_slot(start) == RED)
    {
        revert_colour = RED_;
    }
    else if(gameboard_.which_slot(start) == GREEN)
    {
        revert_colour = GREEN_;
    }
    else
    {
        revert_colour = EMPTY_;
    }
    button_grid_.at(start.x).at(start.y)->setStyleSheet(revert_colour);

    if(gameboard_.is_game_over())
    {
        in_game_ = false;
        basking_in_victory_ = true;

        celebrate_victory();
    }
}

void MainWindow::celebrate_victory()
{
    magicBackground_->setStyleSheet("background-color:purple");

    tickTock_->stop();

    for(unsigned int i = 0; i < COLUMNS; ++i)
    {
        for(unsigned int j = 0; j < ROWS; ++j)
        {
            button_grid_.at(i).at(j)->setDisabled(true);
        }
    }
    points_ -= timerScreen_->intValue();

    if(moves_ == 31)
    {
        magicBackground_->setStyleSheet("background-color:gold");

        points_ += 31;
    }

    if(points_ < 0)
    {
        points_ = 0;
    }
    QString moves_taken = QString::number(moves_);
    QString time_taken  = QString::number(timerScreen_->intValue());
    QString points_got  = QString::number(points_);

    infoBoard_->setText("You won!\n\nTotal moves: "
                        + moves_taken
                        + "\nTime taken: "
                        + time_taken
                        + " sec\n\nTotal points: "
                        + points_got               );
}

void MainWindow::toggle_high_contrast()
{
    if(not high_contrast_)
    {
        high_contrast_ = true;

        RED_        = "background-color:magenta";
        GREEN_      = "background-color:cyan";
        PRESSRED_   = "background-color:darkMagenta";
        PRESSGREEN_ = "background-color:darkCyan";
        EMPTY_      = "background-color:yellow";
        BACKGROUND_ = "background-color:darkGray";
        PAUSED_     = "background-color:black";
        TIMER_      = "background-color:red";
        INFO_       = "background-color:lightGreen";   
    }
    else if(high_contrast_)
    {
        high_contrast_ = false;

        RED_        = "background-color:red";
        GREEN_      = "background-color:lightGreen";
        PRESSRED_   = "background-color:darkRed";
        PRESSGREEN_ = "background-color:green";
        EMPTY_      = "background-color:lightGray";
        BACKGROUND_ = "background-color:lightGray";
        PAUSED_     = "background-color:darkGray";
        TIMER_      = "background-color:black";
        INFO_       = "background-color:white";
    }

    for(unsigned int i = 0; i < COLUMNS; ++i)
    {
        for(unsigned int j = 0; j < ROWS; ++j)
        {
            if(not paused_)
            {
                // Needed conversion.
                int signed_x = (int) i;
                int signed_y = (int) j;

                Point x_and_y = {signed_x, signed_y};

                if(gameboard_.which_slot(x_and_y) == RED)
                {
                    button_grid_.at(i).at(j)->setStyleSheet(RED_);
                }
                else if(gameboard_.which_slot(x_and_y) == GREEN)
                {
                    button_grid_.at(i).at(j)->setStyleSheet(GREEN_);
                }
                else if(gameboard_.which_slot(x_and_y) == EMPTY)
                {
                    button_grid_.at(i).at(j)->setStyleSheet(EMPTY_);
                }
            }
            else if(paused_)
            {
                button_grid_.at(i).at(j)->setStyleSheet(PAUSED_);
            }
        }
    }
    timerScreen_->setStyleSheet(TIMER_);
    infoBoard_->setStyleSheet(INFO_);

    if(not basking_in_victory_)
    {
        magicBackground_->setStyleSheet(BACKGROUND_);
    }
    start_end_.first = null_coordinate;
    start_end_.second = null_coordinate;
}

void MainWindow::toggle_pause()
{
    if(in_game_ and not paused_)
    {
        paused_ = true;
        tickTock_->stop();
        start_end_ = {null_coordinate, null_coordinate};
        infoBoard_->setText("Game paused.");

        for(unsigned int i = 0; i < COLUMNS; ++i)
        {
            for(unsigned int j = 0; j < ROWS; ++j)
            {
                button_grid_.at(i).at(j)->setStyleSheet(PAUSED_);
                button_grid_.at(i).at(j)->setDisabled(true);
            }
        }
    }
    else if(in_game_ and paused_)
    {
        paused_ = false;
        tickTock_->start(1000);
        infoBoard_->setText("Pick your next move...");

        for(unsigned int i = 0; i < COLUMNS; ++i)
        {
            for(unsigned int j = 0; j < ROWS; ++j)
            {
                // Needed conversion.
                int signed_x = (int) i;
                int signed_y = (int) j;

                Point x_and_y = {signed_x, signed_y};

                if(gameboard_.which_slot(x_and_y) == RED)
                {
                    button_grid_.at(i).at(j)->setStyleSheet(RED_);
                }
                else if(gameboard_.which_slot(x_and_y) == GREEN)
                {
                    button_grid_.at(i).at(j)->setStyleSheet(GREEN_);
                }
                else if(gameboard_.which_slot(x_and_y) == EMPTY)
                {
                    button_grid_.at(i).at(j)->setStyleSheet(EMPTY_);
                }

                if(gameboard_.which_slot(x_and_y) != UNUSED)
                {
                    button_grid_.at(i).at(j)->setDisabled(false);
                }
            }
        }
    }
}

void MainWindow::reset_game()
{
    in_game_ = false;
    basking_in_victory_ = false;

    tickTock_->stop();

    gameboard_ = GameBoard(); //Creating a clean logic board.

    moves_ = 0;
    points_ = 0;
    start_end_ = {null_coordinate, null_coordinate};

    magicBackground_->setStyleSheet(BACKGROUND_);

    timerScreen_->display(0);

    QString rules = "Welcome to Polku!\n\nRules:\nThe aim of this game is to "
                    "swap the places of the two coloured block sets. This "
                    "has to be achieved by moving the coloured blocks along "
                    "any free path without other coloured blocks blocking it. "
                    "To get started, first click on a starting block and then "
                    "on a destination block. Good luck!";

    infoBoard_->setText(rules);
}

void MainWindow::run_time()
{
    timerScreen_->display(timerScreen_->intValue() + 1);
}

void MainWindow::quit()
{
    QApplication::quit();
}

