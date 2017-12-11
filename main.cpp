#include <iostream>
#include <fstream>
#include <thread>
#include <string>

using namespace std;



static const char SHIP_SYMBOL = '*';
static const char DEAD_SHIP_SYMBOL = 'x';
static const char SLIP_SYMBOL = '-';

class Field {
    static const int size = 10;
    static const char COMMAND_HELP = 'h';
    static const char COMMAND_ME = 'm';
    static const char COMMAND_ENEMY = 'e';
    static const char COMMAND_FIRE = 'f';
    static const char COMMAND_PASS = 'p';
    static const char COMMAND_BACK = 'b';
    static const char COMMAND_CONTINUE = 'c';
    static constexpr char const *ME_FIELD_TITLE = "Yor Field:: ";
    static constexpr char const *ENEMY_FIELD_TITLE = "Enemy Field:: ";

    char myField[size][size + 1];
    char enemyField[size][size + 1];
    char name[1024];

private:

    void showHelp() {
        cout << "HELP :: " << endl;
        cout << "\t`" << COMMAND_FIRE << "`\t::\tFire!" << endl;
        cout << "\t`" << COMMAND_ME << "`\t::\tshow my field" << endl;
        cout << "\t`" << COMMAND_ENEMY << "`\t::\tshow my enemy" << endl;
        cout << "\t`" << COMMAND_PASS << "`\t::\tPass!" << endl;
        cout << endl;
    }

    void showMyField() {
        showField((char *) ME_FIELD_TITLE, myField);
    }

    void showEnemyField() {
        showField((char *) ENEMY_FIELD_TITLE, enemyField);

    }

    void showField(char *fieldName, char field[][size + 1]) {
        cout << fieldName << endl;
        for (int i = 0; i < size; i++)
            cout << '\t' << field[i] << endl;
        cout << endl;
    }

    bool fire(int &x, int &y) {
        bool isValid = false;
        char userX;
        int userY;
        while (!isValid) {
            cout << "Enter horizontal position(letter) :: ";
            cin >> userX;
            cout << endl;
            cout << "Enter vertical position(number) :: ";
            cin >> userY;
            cout << endl;

            userX = toupper(userX);
            userY = toupper(userY);
            isValid = userX >= 'A' && userX <= ((char) (((int) 'A') + size));
            isValid &= userY >= 1 && userY <= size;
            if (!isValid) {
                cout << "Can not parse position. Please press `" << COMMAND_CONTINUE << "` for try again, or press `"
                     << COMMAND_BACK << "` for back" << endl;
                char answer;
                cin >> answer;
                switch (answer) {
                    case COMMAND_CONTINUE:
                        break;
                    case COMMAND_BACK:
                        return false;
                    default:
                        cout << "Can not parse command `" << answer << "`. So we back" << endl;
                        return false;
                }
            }
        }
        x = ((int) (userX - 'A')) + 1;
        y = userY;
        return true;
    }


public:
    Field() {
        cout << "enter name :: ";
        cin >> name;


        char path[1024];
        cout << "enter path to file :: ";
        cin >> path;
        ifstream source(path);

        char symbol = ' ';
        char *line;
        string strLine;
        for (int l = 0; l < size; l++) {
            if (std::getline(source, strLine))
                line = (char *) strLine.c_str();

            for (int s = 0; s < size + 1; s++) {
                if (s == size)
                    symbol = line[size] = '\0';
                else if (l == 0 && s == 0)
                    symbol = '%';
                else if (l == 0)
                    symbol = (char) ((int) 'A' + s - 1);
                else if (s == 0)
                    symbol = (char) ((int) '0' + l);
                else
                    symbol = '0';

                enemyField[l][s] = symbol;

                if (s < strlen(line) && l > 0 && s > 0)
                    symbol = line[s];
                if(symbol == ' ')
                    symbol = '0';
                myField[l][s] = symbol;
                char c = 'c';
            }
        }
        int i = 10;
    }

    void applayStep(int &x, int &y) {
        showHelp();
        cout << "Enter command, or `" << COMMAND_HELP << "` for show help :: ";
        bool isFinish = false;
        char a;
        while (!isFinish) {
            cin >> a;
            cout << endl;
            switch (a) {
                case COMMAND_HELP:
                    showHelp();
                    break;
                case COMMAND_ME:
                    showMyField();
                    break;
                case COMMAND_ENEMY:
                    showEnemyField();
                    break;
                case COMMAND_PASS:
                    isFinish = true;
                    break;
                case COMMAND_FIRE:
                    isFinish = fire(x, y);
                    if (!isFinish)
                        showHelp();
                    break;
                default:
                    cout << "Can not recognize command. Please enter it again o press `"
                         << COMMAND_HELP << "` for show help" << endl;
            }
        }
    }

    void gotShip(int x,int y){
        enemyField[y][x] = DEAD_SHIP_SYMBOL;
    }

    void failShow(int x,int y){
        enemyField[y][x] = SLIP_SYMBOL;
    }

    char **getPlayerField() {
        return (char **) this->myField;
    }

    char *getName() {
        return name;
    }

    bool atack(int x,int y){
        if(myField[y][x] == SHIP_SYMBOL){
            myField[y][x] = DEAD_SHIP_SYMBOL;
            return true;
        }
        return false;
    }
};

class Broker {
private:
    int FIELD_SIZE = 11;

    mutex mtx;

    Field *players[2];
    int whoWin = -1;

    void playerLoop(Field *player, Field *enemy) {
        while (whoWin < 0) {
            makeStepFor(player, enemy);
        }
    }

    void makeStepFor(Field *player, Field *enemy) {
        mtx.lock();
        cout << "Welcome player `" << player->getName() << "` your current thread id = " << this_thread::get_id()
             << endl;
        int x,y;
        bool isCanFire = true;
        while (isCanFire) {
            x = -1;
            y=-1;
            player->applayStep(x, y);
            if(x == -1 || y == -1)
                break;

            if(enemy->atack(x,y)){
                player->gotShip(x,y);
                cout << "Good shot!! Fire one more time" << endl << endl;
            }
            else{
                player->failShow(x,y);
                isCanFire = false;
                cout << "Sorry, you miss" << endl;
            }
        }
        mtx.unlock();
    }

public:

    Broker() {

        players[0] = new Field();
        players[1] = new Field();

    }

    void startGame() {

        mtx.lock();
        thread th1 = thread(&Broker::playerLoop, this, players[0], players[1]);
        thread th2 = thread(&Broker::playerLoop, this, players[1], players[0]);
        mtx.unlock();

        th1.join();
        th2.join();
    }


};

int main() {
    Broker broker;
    broker.startGame();
    return 0;
}