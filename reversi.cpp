//Created by Dipansh  & Shivansh 


#include <iostream>
#include <vector>
#include <algorithm> //using for sorting
#include <unordered_set>
#include <random>
#include <time.h>
#include <cstdlib> //is this needed?

//TODO: remove AI as a second player
//TODO: implement MCTS for AI

using namespace std;

vector<int> valid_moves();
void flip_tiles(int input);

vector<int> current_valid_vertical_moves;
vector<int> current_valid_horizontal_moves;
vector<int> current_valid_diagonal_moves;
//vector<int> current_all_valid_moves;

static string user = " O ";
static string ai = " X ";
static string allowed_move = " * ";
//if 1 then user turn else ai turn
bool is_user_turn = 1;
bool game_still_running = 1;  //true if game is still running, false otherwise

bool previous_user_passed_turn = 0;

string board[8][8]; //our game board of 8x8

int position_score_board[8][8];

//populate the score boards with points for each position
void populate_score_board()
{
    //corner scores
    position_score_board[0][0] = 3;
    position_score_board[0][7] = 3;
    position_score_board[7][0] = 3;
    position_score_board[7][7] = 3;

    //corner scores
    position_score_board[0][2] = 2;
    position_score_board[0][5] = 2;
    position_score_board[2][0] = 2;
    position_score_board[2][7] = 2;
    position_score_board[5][0] = 2;
    position_score_board[5][7] = 2;
    position_score_board[7][2] = 2;
    position_score_board[7][5] = 2;

    //mid board scores
    //corner scores
    position_score_board[2][2] = 1;
    position_score_board[2][5] = 1;
    position_score_board[5][2] = 1;
    position_score_board[5][5] = 1;

    //avoid positions around corners
    position_score_board[0][1] = -1;
    position_score_board[0][6] = -1;
    position_score_board[1][0] = -1;
    position_score_board[1][1] = -1;
    position_score_board[1][6] = -1;
    position_score_board[1][7] = -1;
    position_score_board[6][0] = -1;
    position_score_board[6][1] = -1;
    position_score_board[6][6] = -1;
    position_score_board[6][7] = -1;
    position_score_board[7][1] = -1;
    position_score_board[7][6] = -1;

    for(int i = 0; i < 8 ; i++)
    {
        for(int j = 0; j < 8 ; j++)
        {
            if((position_score_board[i][j] != 3) && (position_score_board[i][j] != 2) && (position_score_board[i][j] != 1) && (position_score_board[i][j] != -1))
            {
                position_score_board[i][j] = 0;
            }
        }
    }

    return;
}

vector<int> scout_towards_bottom_right(int index);
vector<int> scout_towards_bottom_left(int index);
vector<int> scout_towards_top_right(int index);
vector<int> scout_towards_top_left(int index);

//initialize empty board
void initialize_board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            board[i][j] = " . ";
        }
    }
    board[3][3] = user;
    board[4][4] = user;
    board[3][4] = ai;
    board[4][3] = ai;

}

//print the board
void print_board()
{

    for (int i = 0; i < 8; i++) {
        cout << "\n";
        for (int j = 0; j < 8; j++) {
            cout << board[i][j];
        }
        //this loop prints helping numbers
        cout << "     ";
        for (int j = 0; j < 8; j++) {
            cout << " " << i << j << " ";
        }
    }
    cout << "\n\n";
}

void print_board_with_valid_moves()
{
    //cout << "calling valid_moves in print board with valid moves" << endl;
    vector<int> current_valid_moves = valid_moves();
    sort(current_valid_moves.begin(), current_valid_moves.end());
    //cout << "back from valid_moves in print board\n";


    cout << "\n   0  1  2  3  4  5  6  7";
    for (int i = 0; i < 8; i++)
    {
        cout<<"\n"<<i<<" ";
        for (int j = 0; j < 8; j++)
        {
//            cout<<j<<" ";
            int input = (i * 10 + j);
            if (binary_search(current_valid_moves.begin(), current_valid_moves.end(), input))
            {
                cout << " * ";
            }
            else {
                cout << board[i][j];
            }
        }
        //this loop prints helping numbers
    }
    cout << "\n\n";
}

void remove_duplicate_valid_moves(vector<int>& v)
{
    vector<int>::iterator itr = v.begin();
    unordered_set<int> s;

    for (auto curr = v.begin(); curr != v.end(); ++curr)
    {
        if (s.insert(*curr).second)
            *itr++ = *curr;
    }
    v.erase(itr, v.end());
}

//returns the number of tiles for the current player
int num_tiles()
{
    string curr_player = " O ";

    if(!is_user_turn)
    {
        curr_player = " X ";
    }

    int count = 0;

    for (int i = 0; i <= 7 ; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            if(board[i][j] == curr_player)
            {
                count++;
            }
        }
    }
    return count;
}

//still untested
bool check_if_over()
{
    //TODO: check all stopping conditions here, if stops then change game_still_running to False

    //checks if board is full

    string p = user;

    if(!is_user_turn)
    {
        p = ai;
    }

    vector<int> moves = valid_moves();
    int full = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == " . ")
            {
                continue;
            }
            else
            {
                full++;
            }
        }
    }

    //returns 1 if board is full
    if (full == 64)
    {
        game_still_running = 0;
        return 1;
    }

    //cout<<"current all valid is empty: "<<moves.empty()<<endl;

    //for(auto i:moves)
    //{
    //    cout<<i<<endl;
    //}

    if (moves.empty())
    {
        //cout<<"inside current valid moves if condition"<<endl;
        //cout<<"current player: "<<is_user_turn<<" number of tiles of current player"<<num_tiles()<<endl;
        if(num_tiles() == 0)
        {
            //cout<<"user turn: "<<is_user_turn<<endl;
            //cout<<"num tiles = 0\n";
            game_still_running = 0;
            return 1;
        }

        //if ()
        if (previous_user_passed_turn == 1)
        {
            game_still_running = 0;
            return 1;
        }

        else
        {
            previous_user_passed_turn = 1;
            cout<<" Player : "<< p << " has to SKIP a turn\n";
            return 0;
        }
    }

    previous_user_passed_turn = 0;
    return 0;
}

void print_winner()
{
    int user_count = 0;
    int ai_count = 0;
    int blank_space_count = 0;

    for (int i = 0; i <= 7 ; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            if(board[i][j] == user)
            {
                user_count++;
            }

            else if(board[i][j] == ai)
            {
                ai_count++;
            }

            else
            {
                blank_space_count++;
            }
        }
    }

    if (user_count < ai_count)
    {
        cout <<"The AI won the game!\n";
        cout <<" AI had "<<ai_count<<" tiles\n";
        return;
    }

    else if(user_count > ai_count)
    {
        cout<<"The PLAYER won the game!\n";
        cout <<"Player had "<<user_count<<" tiles\n";
        return;
    }

    else if (user_count == ai_count)
    {
        cout<<"The Game was a TIE.\n";
        return;
    }
}

string get_winner()
{
    int user_count = 0;
    int ai_count = 0;

    for (int i = 0; i <= 7 ; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            if(board[i][j] == user)
            {
                user_count++;
            }
            if(board[i][j] == ai)
            {
                ai_count++;
            }
        }
    }
    if (user_count < ai_count)
    {
        //cout <<"The AI won the game!\n";
        //cout <<" AI had "<<ai_count<<" tiles\n";
        return ai;
    }

    else if(user_count > ai_count)
    {
        //cout<<"The PLAYER won the game!\n";
        //cout <<"Player had "<<user_count<<" tiles\n";
        return user;
    }

    return "tie";
}

vector<int> check_horizontal(string current_turn) {
    //return vector of horizontal valid moves
    vector<int> ret_val;

    //player_one is the one we need to find the valid moves for
    string player_one;
    string player_two;

    if (current_turn == user) {
        player_one = user;
        player_two = ai;
    }
    else {
        player_one = ai;
        player_two = user;
    }

    //search the board for player_one, find valid moves and append it in ret_val
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            //checking on the right side
            if (board[i][j] == " . " && board[i][j + 1] == player_two && j < 6) {
                int another_temp = j + 1;
                while (another_temp < 7) {
                    if (board[i][another_temp + 1] == " . ") {
                        break;
                    }
                    if (board[i][another_temp + 1] == player_two) {
                        another_temp++;
                        continue;
                    }
                    if (board[i][another_temp + 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_j]=allowed_move;//TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //checking on the left side
            if (board[i][j] == " . " && board[i][j - 1] == player_two && j > 1) {
                int another_temp = j - 1;
                while (another_temp > 0) {
                    if (board[i][another_temp - 1] == " . ") {
                        break;
                    }
                    if (board[i][another_temp - 1] == player_two) {
                        another_temp--;
                        continue;
                    }
                    if (board[i][another_temp - 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_k]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }
        }
    }
    current_valid_horizontal_moves = ret_val;
    sort(current_valid_horizontal_moves.begin(), current_valid_horizontal_moves.end());
    return  ret_val;
}

vector<int> check_vertical(string current_turn) {
    //return vector of vertical valid moves
    vector<int> ret_val;

    //player_one is the one we need to find the valid moves for
    string player_one;
    string player_two;

    if (current_turn == user) {
        player_one = user;
        player_two = ai;
    }
    else {
        player_one = ai;
        player_two = user;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            //check down
            if (board[i][j] == " . " && board[i + 1][j] == player_two && i < 6) {

                int another_temp = i + 1;
                while (another_temp < 7) {
                    if (board[another_temp + 1][j] == " . ") {
                        break;
                    }
                    if (board[another_temp + 1][j] == player_two) {
                        another_temp++;
                        continue;
                    }
                    if (board[another_temp + 1][j] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_j]=allowed_move;//TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check above
            if (board[i][j] == " . " && board[i - 1][j] == player_two && i > 1) {
                int another_temp = i - 1;
                while (another_temp > 0) {
                    if (board[another_temp - 1][j] == " . ") {
                        break;
                    }
                    if (board[another_temp - 1][j] == player_two) {
                        another_temp--;
                        continue;
                    }
                    if (board[another_temp - 1][j] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_k]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

        }
    }
    current_valid_vertical_moves = ret_val;
    sort(current_valid_vertical_moves.begin(), current_valid_vertical_moves.end());
    return ret_val;

}

vector<int> check_diagonal(string current_turn) {
    //return vector of diagonal valid moves

    vector<int> ret_val;

    //player_one is the one we need to find the valid moves for
    string player_one;
    string player_two;

    if (current_turn == user) {
        player_one = user;
        player_two = ai;
    }
    else {
        player_one = ai;
        player_two = user;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            //            check up right (finds user)
            if (board[i][j] == " . " && board[i - 1][j + 1] == player_two && i > 1 && j < 6) {
                int another_temp_i = i - 1;
                int another_temp_j = j + 1;
                while (another_temp_i > 0 && another_temp_j < 7) {
                    if (board[another_temp_i - 1][another_temp_j + 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i - 1][another_temp_j + 1] == player_two) {
                        another_temp_j++;
                        another_temp_i--;
                        continue;
                    }
                    if (board[another_temp_i - 1][another_temp_j + 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check down right
            if (board[i][j] == " . " && board[i + 1][j + 1] == player_two && i < 6 && j < 6) {
                int another_temp_i = i + 1;
                int another_temp_j = j + 1;
                while (another_temp_i < 7 && another_temp_j < 7) {
                    if (board[another_temp_i + 1][another_temp_j + 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i + 1][another_temp_j + 1] == player_two) {
                        another_temp_j++;
                        another_temp_i++;
                        continue;
                    }
                    if (board[another_temp_i + 1][another_temp_j + 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check down left
            if (board[i][j] == " . " && board[i + 1][j - 1] == player_two && j > 1 && i < 6) {
                int another_temp_i = i + 1;
                int another_temp_j = j - 1;
                while (another_temp_i < 7 && another_temp_j > 0) {
                    if (board[another_temp_i + 1][another_temp_j - 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i + 1][another_temp_j - 1] == player_two) {
                        another_temp_j--;
                        another_temp_i++;
                        continue;
                    }
                    if (board[another_temp_i + 1][another_temp_j - 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check up left
            if (board[i][j] == " . " && board[i - 1][j - 1] == player_two && j > 1 && i > 1) {
                int another_temp_i = i - 1;
                int another_temp_j = j - 1;
                while (another_temp_i > 0 && another_temp_j > 0) {
                    if (board[another_temp_i - 1][another_temp_j - 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i - 1][another_temp_j - 1] == player_two) {
                        another_temp_j--;
                        another_temp_i--;
                        continue;
                    }
                    if (board[another_temp_i - 1][another_temp_j - 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }
        }
    }
    current_valid_diagonal_moves = ret_val;
    sort(current_valid_diagonal_moves.begin(), current_valid_diagonal_moves.end());

    //    cout<<"FROM DIAGONAL: ";
    //
    //        for (auto it : ret_val)
    //        cout << it << " ";

    return  ret_val;
}

vector<int> current_check_horizontal(string current_turn) {
    //return vector of horizontal valid moves
    vector<int> ret_val;

    //player_one is the one we need to find the valid moves for
    string player_one;
    string player_two;

    if (current_turn == user) {
        player_one = user;
        player_two = ai;
    }
    else {
        player_one = ai;
        player_two = user;
    }

    //search the board for player_one, find valid moves and append it in ret_val
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            //checking on the right side
            if (board[i][j] == " . " && board[i][j + 1] == player_two && j < 6) {
                int another_temp = j + 1;
                while (another_temp < 7) {
                    if (board[i][another_temp + 1] == " . ") {
                        break;
                    }
                    if (board[i][another_temp + 1] == player_two) {
                        another_temp++;
                        continue;
                    }
                    if (board[i][another_temp + 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_j]=allowed_move;//TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //checking on the left side
            if (board[i][j] == " . " && board[i][j - 1] == player_two && j > 1) {
                int another_temp = j - 1;
                while (another_temp > 0) {
                    if (board[i][another_temp - 1] == " . ") {
                        break;
                    }
                    if (board[i][another_temp - 1] == player_two) {
                        another_temp--;
                        continue;
                    }
                    if (board[i][another_temp - 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_k]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }
        }
    }
    //current_valid_horizontal_moves = ret_val;
    //sort(current_valid_horizontal_moves.begin(), current_valid_horizontal_moves.end());
    return  ret_val;
}

vector<int> current_check_vertical(string current_turn) {
    //return vector of vertical valid moves
    vector<int> ret_val;

    //player_one is the one we need to find the valid moves for
    string player_one;
    string player_two;

    if (current_turn == user) {
        player_one = user;
        player_two = ai;
    }
    else {
        player_one = ai;
        player_two = user;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            //check down
            if (board[i][j] == " . " && board[i + 1][j] == player_two && i < 6) {

                int another_temp = i + 1;
                while (another_temp < 7) {
                    if (board[another_temp + 1][j] == " . ") {
                        break;
                    }
                    if (board[another_temp + 1][j] == player_two) {
                        another_temp++;
                        continue;
                    }
                    if (board[another_temp + 1][j] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_j]=allowed_move;//TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check above
            if (board[i][j] == " . " && board[i - 1][j] == player_two && i > 1) {
                int another_temp = i - 1;
                while (another_temp > 0) {
                    if (board[another_temp - 1][j] == " . ") {
                        break;
                    }
                    if (board[another_temp - 1][j] == player_two) {
                        another_temp--;
                        continue;
                    }
                    if (board[another_temp - 1][j] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][temp_k]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

        }
    }
    //current_valid_vertical_moves = ret_val;
    //sort(current_valid_vertical_moves.begin(), current_valid_vertical_moves.end());
    return ret_val;

}

vector<int> current_check_diagonal(string current_turn) {
    //return vector of diagonal valid moves

    vector<int> ret_val;

    //player_one is the one we need to find the valid moves for
    string player_one;
    string player_two;

    if (current_turn == user) {
        player_one = user;
        player_two = ai;
    }
    else {
        player_one = ai;
        player_two = user;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            //            check up right (finds user)
            if (board[i][j] == " . " && board[i - 1][j + 1] == player_two && i > 1 && j < 6) {
                int another_temp_i = i - 1;
                int another_temp_j = j + 1;
                while (another_temp_i > 0 && another_temp_j < 7) {
                    if (board[another_temp_i - 1][another_temp_j + 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i - 1][another_temp_j + 1] == player_two) {
                        another_temp_j++;
                        another_temp_i--;
                        continue;
                    }
                    if (board[another_temp_i - 1][another_temp_j + 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check down right
            if (board[i][j] == " . " && board[i + 1][j + 1] == player_two && i < 6 && j < 6) {
                int another_temp_i = i + 1;
                int another_temp_j = j + 1;
                while (another_temp_i < 7 && another_temp_j < 7) {
                    if (board[another_temp_i + 1][another_temp_j + 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i + 1][another_temp_j + 1] == player_two) {
                        another_temp_j++;
                        another_temp_i++;
                        continue;
                    }
                    if (board[another_temp_i + 1][another_temp_j + 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check down left
            if (board[i][j] == " . " && board[i + 1][j - 1] == player_two && j > 1 && i < 6) {
                int another_temp_i = i + 1;
                int another_temp_j = j - 1;
                while (another_temp_i < 7 && another_temp_j > 0) {
                    if (board[another_temp_i + 1][another_temp_j - 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i + 1][another_temp_j - 1] == player_two) {
                        another_temp_j--;
                        another_temp_i++;
                        continue;
                    }
                    if (board[another_temp_i + 1][another_temp_j - 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }

            //check up left
            if (board[i][j] == " . " && board[i - 1][j - 1] == player_two && j > 1 && i > 1) {
                int another_temp_i = i - 1;
                int another_temp_j = j - 1;
                while (another_temp_i > 0 && another_temp_j > 0) {
                    if (board[another_temp_i - 1][another_temp_j - 1] == " . ") {
                        break;
                    }
                    if (board[another_temp_i - 1][another_temp_j - 1] == player_two) {
                        another_temp_j--;
                        another_temp_i--;
                        continue;
                    }
                    if (board[another_temp_i - 1][another_temp_j - 1] == player_one) {
                        int valid_move_two_digit = (i * 10) + j;
                        ret_val.push_back(valid_move_two_digit);
                        //                            board[i][j]=allowed_move; //TODO: Delete this, it's only for testing
                        break;
                    }
                }
            }
        }
    }
    //current_valid_diagonal_moves = ret_val;
    //sort(current_valid_diagonal_moves.begin(), current_valid_diagonal_moves.end());

    //    cout<<"FROM DIAGONAL: ";
    //
    //        for (auto it : ret_val)
    //        cout << it << " ";

    return  ret_val;
}


//returns a **vector** of all valid moves at current board position
vector<int> valid_moves() {

    string current_turn;

    if (is_user_turn == 1) {

        current_turn = user;
    }
    else {
        current_turn = ai;

    }
    vector<int> horizontal_valid_moves = check_horizontal(current_turn);
    vector<int> vertical_valid_moves = check_vertical(current_turn);
    vector<int> diagonal_valid_moves = check_diagonal(current_turn);
    //current_valid_diagonal_moves = diagonal_valid_moves;

    vector<int> total_valid_moves;
    total_valid_moves.insert(total_valid_moves.end(), horizontal_valid_moves.begin(), horizontal_valid_moves.end());
    total_valid_moves.insert(total_valid_moves.end(), vertical_valid_moves.begin(), vertical_valid_moves.end());
    total_valid_moves.insert(total_valid_moves.end(), diagonal_valid_moves.begin(), diagonal_valid_moves.end());

    //remove_duplicate_valid_moves(current_all_valid_moves);
    //current_all_valid_moves = total_valid_moves;

    //cout<<"RETURNING FROM VALID MOVES\n";
    remove_duplicate_valid_moves(total_valid_moves);

    return total_valid_moves;
}

vector<int> curr_valid_moves() {

    string current_turn;

    if (is_user_turn == 1) {

        current_turn = user;
    }
    else {
        current_turn = ai;

    }
    vector<int> current_horizontal_valid_moves = check_horizontal(current_turn);
    vector<int> current_vertical_valid_moves = check_vertical(current_turn);
    vector<int> current_diagonal_valid_moves = check_diagonal(current_turn);
    //current_valid_diagonal_moves = diagonal_valid_moves;

    vector<int> total_valid_moves;
    total_valid_moves.insert(total_valid_moves.end(), current_horizontal_valid_moves.begin(), current_horizontal_valid_moves.end());
    total_valid_moves.insert(total_valid_moves.end(), current_vertical_valid_moves.begin(), current_vertical_valid_moves.end());
    total_valid_moves.insert(total_valid_moves.end(), current_diagonal_valid_moves.begin(), current_diagonal_valid_moves.end());

    //remove_duplicate_valid_moves(current_all_valid_moves);
    //current_all_valid_moves = total_valid_moves;

    //cout<<"RETURNING FROM VALID MOVES\n";
    remove_duplicate_valid_moves(total_valid_moves);

    return total_valid_moves;
}

void mcts();

//changes the board according to the user input
//TODO: implement the algorithm to change the board according to the rules after user turn
void user_turn(int input) {
    //TODO: error handling for invalid input

    //process user input and change the tile
    int first_digit = input / 10;
    int second_digit = input % 10;

    vector<int> current_valid_moves = valid_moves();
    sort(current_valid_moves.begin(), current_valid_moves.end());

    if (binary_search(current_valid_moves.begin(), current_valid_moves.end(), input)) {
        flip_tiles(input);
        //board[first_digit][second_digit] = user; //TODO: delete this later
        //cout << "back in USER turn" << endl;
        //is_user_turn = 0;     //change it to ai_turn
    }
    else {
        cout << "Invalid Input!\n Select a valid position\n";
    }

}

void mcts_heuristics();

//this function will be called to when it's ai turn
void ai_turn()
{
    //mcts();
    mcts_heuristics();
    //flip_tiles(mcts());
    is_user_turn = 1;

    //print_board_with_valid_moves();
}

//called by flipping..can also be later used for heuristics
vector<int> scout_towards_bottom_right(int index) // needs fixing
{
    string player_char = " O ";
    vector<int> flippable;

    if (!is_user_turn)
    {
        player_char = " X ";
    }

    int curr_ind = index;
    int input_row = index / 10;
    int input_column = index % 10;

    //std::cout << "inp row: " << input_row << "\t inp col: " << input_column << endl;

    if (board[input_row + 1][input_column + 1] != player_char && board[input_row + 1][input_column + 1] != " . ")
    {
        int i = input_row + 1;
        int j = input_column + 1;

        //std::cout << "in the first if condition" << endl;
        //std::cout << "i = " << i << "\tj = " << j << endl;

        for (; i <= 6 && j <= 6; i++, j++) //chaged logic stuff here
        {
            //std::cout << "in the for loop:" << endl;
            if (board[i][j] != player_char && board[i][j] != " . ")
            {
                //std::cout << "in the inner if condition\n" << "i : " << i << "\tj : " << j << endl;
                if (board[i + 1][j + 1] == player_char)
                {
                    curr_ind = i * 10 + j;
                    //std::cout << "in the CORE if condition\n" << "corr_ind : " << curr_ind << endl;
                    break;
                }
            }
        }

        //std::cout << "out of the loop" << endl;

        if (curr_ind != index)
        {
            int curr_row = curr_ind / 10;
            int curr_col = curr_ind % 10;
            //flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            //std::cout << "curr_ind != index is TRUE" << endl;
            for (; (input_row <= (curr_row)) && (input_column <= (curr_col)); curr_row--, curr_col--)
            {
                flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            }
        }
        //return flippable;
    }

    // delete this later
    //std::cout << "printing flippable indexes in stbr: " << endl;
    //for (auto it : flippable)
    //{
    //    std::cout << it << endl;
    //}
    return flippable;
}

//called by flipping..can also be later used for heuristics
vector<int> scout_towards_top_left(int index)
{
    string player_char = " O ";
    vector<int> flippable;

    if (!is_user_turn)
    {
        player_char = " X ";
    }

    int curr_ind = index;
    int input_row = index / 10;
    int input_column = index % 10;

    //std::cout << "inp row: " << input_row << "\t inp col: " << input_column << endl;

    if (board[input_row - 1][input_column - 1] != player_char && board[input_row - 1][input_column - 1] != " . ")
    {
        int i = input_row - 1;
        int j = input_column - 1;

        //std::cout << "in the first if condition" << endl;
        //std::cout << "i = " << i << "\tj = " << j << endl;

        for (; i >= 1 && j >= 1; i--, j--) //chaged logic stuff here
        {
            //std::cout << "in the for loop:" << endl;
            if (board[i][j] != player_char && board[i][j] != " . ")
            {
                //std::cout << "in the inner if condition\n" << "i : " << i << "\tj : " << j << endl;
                if (board[i - 1][j - 1] == player_char)
                {
                    curr_ind = i * 10 + j;
                    //std::cout << "in the CORE if condition\n" << "corr_ind : " << curr_ind << endl;
                    break;
                }
            }
        }

        //std::cout << "out of the loop" << endl;

        if (curr_ind != index)
        {
            int curr_row = curr_ind / 10;
            int curr_col = curr_ind % 10;
            //flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            //std::cout << "curr_ind != index is TRUE" << endl;
            for (; (input_row >= (curr_row)) && (input_column >= (curr_col)); curr_row++, curr_col++)
            {
                flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            }
        }
        //return flippable;
    }

    // delete this later
    //std::cout << "printing flippable indexes in sttl: " << endl;
    //for (auto it : flippable)
    //{
    //    std::cout << it << endl;
    //}
    return flippable;
}

//called by flipping..can also be later used for heuristics
//vector<int> scout_towards_bottom_left(int index)
vector<int> scout_towards_top_right(int index)
{
    string player_char = " O ";
    vector<int> flippable;

    if (!is_user_turn)
    {
        player_char = " X ";
    }

    int curr_ind = index;
    int input_row = index / 10;//4
    int input_column = index % 10;//2

    //std::cout << "inp row: " << input_row << "\t inp col: " << input_column << endl;

    if (board[input_row - 1][input_column + 1] != player_char && board[input_row - 1][input_column + 1] != " . ")
    {
        int i = input_row - 1;//3
        int j = input_column + 1;//3

        //std::cout << "in the first if condition" << endl;
        //std::cout << "i = " << i << "\tj = " << j << endl;

        for (; i >= 1 && j <= 6; i--, j++) //chaged logic stuff here
        {
            //std::cout << "in the for loop:" << endl;
            if (board[i][j] != player_char && board[i][j] != " . ")
            {
                //std::cout << "in the inner if condition\n" << "i : " << i << "\tj : " << j << endl;
                if (board[i - 1][j + 1] == player_char)
                {
                    curr_ind = i * 10 + j;
                    //std::cout << "in the CORE if condition\n" << "corr_ind : " << curr_ind << endl;
                    break;
                }
            }
        }

        //std::cout << "out of the loop" << endl;

        if (curr_ind != index)//33
        {
            int curr_row = curr_ind / 10;
            int curr_col = curr_ind % 10;
            //flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            //std::cout << "curr_ind != index is TRUE" << endl;
            for (; (input_row >= (curr_row)) && (input_column <= (curr_col)); curr_row++, curr_col--)
            {
                flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            }
        }
        //return flippable;
    }

    // delete this later
    //if(flippable.size()>0)
    //{
    //    std::cout << "printing flippable indexes in sttr: " << endl;
    //}
    //for (auto it : flippable)
    //{
    //    std::cout << it << endl;
    //}
    return flippable;
}

//called by flipping..can also be later used for heuristics
//WORKS FINALLY
vector<int> scout_towards_bottom_left(int index)
{
    string player_char = " O ";
    vector<int> flippable;

    if (!is_user_turn)
    {
        player_char = " X ";
    }

    int curr_ind = index;//24
    int input_row = index / 10;//2
    int input_column = index % 10;//4

    //cout << " input_row : " << input_row;
    //cout << " input_column : " << input_column;

    //std::cout << "inp row: " << input_row << "\t inp col: " << input_column << endl;

    if (board[input_row + 1][input_column - 1] != player_char && board[input_row + 1][input_column - 1] != " . ")
    {
        //cout << "in the loop" << endl;
        int i = input_row + 1;//3
        int j = input_column - 1;//3

        //std::cout << "in the first if condition" << endl;
        //std::cout << "i = " << i << "\tj = " << j << endl;

        for (; i <= 6 && j >= 1; i++, j--) //chaged logic stuff here
        {
            //std::cout << "in the for loop:" << endl;
            if (board[i][j] != player_char && board[i][j] != " . ")
            {
                //std::cout << "in the inner if condition\n" <<"i : "<<i<<"\tj : "<<j<< endl;
                if (board[i + 1][j - 1] == player_char)
                {
                    curr_ind = i * 10 + j;
                    //std::cout << "in the CORE if condition\n" << "corr_ind : " << curr_ind << endl;
                    break;
                }
            }
        }

        //std::cout << "out of the loop" << endl;

        if (curr_ind != index)
        {
            int curr_row = curr_ind / 10;//3
            int curr_col = curr_ind % 10;//3
            //flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            //std::cout << "curr_ind != index is TRUE" << endl;
            for (; (input_row <= (curr_row)) && (input_column >= (curr_col)); curr_row--, curr_col++)
            {
                flippable.insert(flippable.end(), (curr_row * 10 + curr_col));
            }
        }
        //return flippable;
    }

    // delete this later
    //std::cout << "printing flippable indexes in stbl: " << endl;
    //for (auto it : flippable)
    //{
    //    std::cout << it << endl;
    //}
    return flippable;
}

//DONT TOUCH THIS!!!!!
void flip_tiles(int input) {
    //TODO: make func to flip tiles

    //cout << "in flip tiles" <<endl;
    //cout<<"input : "<<input <<endl;
    int input_row = input / 10;
    int input_column = input % 10;

    string player_char = " O ";

    if (!is_user_turn)
    {
        player_char = " X ";
    }

    //if condition to check if in VALID HORIZONTAL moves
    if (binary_search(current_valid_horizontal_moves.begin(), current_valid_horizontal_moves.end(), input))
    {
        //left 8x2 sub-board
        if (input_column == 0 || input_column == 1)
        {
            for (int i = input_column; i < 7; i++)
            {
                //cout << "Loop 1" << endl;
                board[input_row][i] = player_char;
                if (board[input_row][i + 1] == player_char)
                {
                    break;
                    //return;
                }
            }
        }

        else if (input_column == 6 || input_column == 7)
        {
            for (int i = input_column; i > 0; i--)
            {
                //cout << "Loop 2" << endl;
                board[input_row][i] = player_char;
                if (board[input_row][i - 1] == player_char)
                {
                    break;
                    //return;
                }
            }
        }

        else if (input_column >= 2 && input_column <= 5)
        {
            int curr_ind = input;//62
            if (board[input_row][input_column - 1] == " . ")
            {
                for (int i = input_column; i < 7; i++)
                {
                    //cout << "Loop 3" << endl;
                    board[input_row][i] = player_char;
                    if (board[input_row][i + 1] == player_char)
                    {
                        break;
                        //return;
                    }
                }
            }

            if (board[input_row][input_column + 1] == " . ")
            {
                for (int i = input_column; i > 0; i--)
                {
                    //cout << "Loop 4" << endl;
                    board[input_row][i] = player_char;
                    if (board[input_row][i - 1] == player_char)
                    {
                        break;
                        //return;
                    }
                }
            }
            //change
            //int curr_ind = input;
            //if none of the adjascent is blank
            if (board[input_row][input_column - 1] != " . " && board[input_row][input_column - 1] != player_char)
            {
                for (int i = input_column - 1; i > 0; i--)//i=1
                {
                    if (board[input_row][i] != player_char)
                    {
                        if (board[input_row][i - 1] == player_char)
                        {
                            curr_ind = input_row * 10 + i;
                            break;
                        }
                    }
                }
                //cout<<"curr_ind : "<<curr_ind<<endl;

                if (curr_ind != input)
                {
                    int curr_col = curr_ind % 10;//2
                    for (; curr_col <= input_column; curr_col++)
                    {
                        board[input_row][curr_col] = player_char;
                    }
                }
            }

            //change
            if (board[input_row][input_column + 1] != " . " && board[input_row][input_column + 1] != player_char)
            {
                for (int i = input_column + 1; i < 7; i++)//i = 4
                {
                    if (board[input_row][i] != player_char)
                    {
                        if (board[input_row][i + 1] == player_char)
                        {
                            curr_ind = input_row * 10 + i;
                            break;
                        }
                    }
                }

                //cout<<"curr_ind: "<<curr_ind<<endl;
                if (curr_ind != input)
                {
                    int curr_col = curr_ind % 10;
                    for (; curr_col >= input_column; curr_col--)
                    {
                        board[input_row][curr_col] = player_char;
                    }
                }
            }
        }
    }

    //if condition to check if in VALID VERTICAL moves

    if (binary_search(current_valid_vertical_moves.begin(), current_valid_vertical_moves.end(), input))
    {
        if (input_row == 0 || input_row == 1)
        {
            for (int i = input_row; i < 7; i++)
            {
                //cout << "Loop 5" << endl;
                board[i][input_column] = player_char;
                if (board[i + 1][input_column] == player_char)
                {
                    break;
                    //return;
                }
            }
        }

        else if (input_row == 6 || input_row == 7)
        {
            for (int i = input_row; i > 0; i--)
            {
                //cout << "Loop 6" << endl;
                board[i][input_column] = player_char;
                if (board[i - 1][input_column] == player_char)
                {
                    //cout << "HERE" << endl;
                    break;
                    //return;
                }
            }
            //print_board_with_valid_moves();
            //cout<<"OUT HERE"<<endl;
        }

        else if (input_row >= 2 && input_row <= 5)
        {
            //cout << "in HERE" << endl;
            int curr_ind = input;

            //start going the opposite direction if one of them is blank space
            if (board[input_row - 1][input_column] == " . ")
            {
                for (int i = input_row; i < 7; i++)
                {
                    //cout << "Loop 7" << endl;
                    board[i][input_column] = player_char;
                    if (board[i + 1][input_column] == player_char)
                    {
                        break;
                        //return;
                    }
                }
                //cout<<"YO\n";
            }

            if (board[input_row + 1][input_column] == " . ")
            {
                for (int i = input_row; i > 0; i--)
                {
                    //cout << "Loop 8" << endl;
                    board[i][input_column] = player_char;
                    if (board[i - 1][input_column] == player_char)
                    {
                        break;
                        //return;
                    }
                }
            }

            //int curr_ind = input;
            //if none of the adjascent is blank
            if (board[input_row - 1][input_column] != " . " && board[input_row - 1][input_column] != player_char)
            {
                for (int i = input_row - 1; i > 0; i--)
                {
                    if (board[i][input_column] != player_char)
                    {
                        if (board[i - 1][input_column] == player_char)
                        {
                            curr_ind = i * 10 + input_column;
                            break;
                        }
                    }
                }

                if (curr_ind != input)
                {
                    int curr_row = curr_ind / 10;
                    for (; curr_row <= input_row; curr_row++)
                    {
                        board[curr_row][input_column] = player_char;
                    }
                }
            }

            if (board[input_row + 1][input_column] != " . " && board[input_row + 1][input_column] != player_char)
            {
                for (int i = input_row + 1; i < 7; i++)// fault was here
                {
                    if (board[i][input_column] != player_char)
                    {
                        if (board[i + 1][input_column] == player_char)
                        {
                            curr_ind = i * 10 + input_column;
                            break;
                        }
                    }
                }

                if (curr_ind != input)
                {
                    int curr_row = curr_ind / 10;
                    for (; curr_row >= input_row; curr_row--)
                    {
                        board[curr_row][input_column] = player_char;
                    }
                }
            }
        }
        //cout<<"OUT HERE 2"<<endl;
    }

    //cout<<"reaching if for diagonal flips\n";
    //if condition to check if in VALID DIAGONAL moves
    if (binary_search(current_valid_diagonal_moves.begin(), current_valid_diagonal_moves.end(), input))
    {
        //these vectors scout the 4 directions to find the tiles to flip
        vector<int> diag_moves;
        vector<int> scout_tl; //scout the top left side
        vector<int> scout_tr; //scout the top right side
        vector<int> scout_bl; //scout the bottom left side
        vector<int> scout_br; //scout the bottom right side

        //cout<<"input_row in diagonal: "<<input_row<<" and input col "<<input_column<<endl;
        //look at the sub-board between 2,2 and 5,5
        if (input_row >= 2 && input_column >= 2 && input_row <= 5 && input_column <= 5)
        {

            scout_br = scout_towards_bottom_right(input);
            scout_tl = scout_towards_top_left(input);
            scout_bl = scout_towards_bottom_left(input);
            scout_tr = scout_towards_top_right(input);

            diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());
            diag_moves.insert(diag_moves.end(), scout_tl.begin(), scout_tl.end());
            diag_moves.insert(diag_moves.end(), scout_bl.begin(), scout_bl.end());
            diag_moves.insert(diag_moves.end(), scout_tr.begin(), scout_tr.end());

            //cout << "total diag_ moves size : "<<diag_moves.size()<<endl;

            for (auto it : diag_moves)
            {
                //cout << "Loop 9" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the middle diagonal box\n";
            return;
        }

            //look at the sub-board between 0,0 and 1,1
        else if (input == 00 || input == 01 || input == 10 || input == 11)
        {
            scout_br = scout_towards_bottom_right(input);

            //diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());
            diag_moves = scout_br;


            for (auto it : diag_moves)
            {
                //cout << "Loop 10" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the top-left diagonal box\n";
            return;
        }

            //look at the sub-board between 0,6 and 1,7
        else if (input == 06 || input == 07 || input == 16 || input == 17)
        {
            scout_bl = scout_towards_bottom_left(input);

            //diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());
            diag_moves = scout_bl;

            for (auto it : diag_moves)
            {
                //cout << "Loop 11" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the top-right diagonal box\n";
            return;
        }

            //look at the sub-board between 6,0 and 7,1
        else if (input == 60 || input == 61 || input == 70 || input == 71)
        {
            scout_tr = scout_towards_top_right(input);

            //diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());
            diag_moves = scout_tr;

            for (auto it : diag_moves)
            {
                //cout << "Loop 12" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the bottom-left diagonal box\n";
            return;
        }

            //look at the sub-board between 6,6 and 7,7
        else if (input == 66 || input == 67 || input == 76 || input == 77)
        {
            scout_tl = scout_towards_top_left(input);

            //diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());
            diag_moves = scout_tl;

            for (auto it : diag_moves)
            {
                //cout << "Loop 13" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the bottom-right diagonal box\n";
            return;
        }

        // 2x2 corner boards done

        /*now taking care of 2x4 edge boards*/

        //top side board case

        if ((input_row == 0 && input_column >= 2 && input_column <= 5) || (input_row == 1 && input_column >= 2 && input_column <= 5))
        {
            //cout<<"calling stbl\n";
            scout_bl = scout_towards_bottom_left(input);
            //cout<<"returning from bl\n";

            //cout<<"calling stbr\n";
            scout_br = scout_towards_bottom_right(input);
            //cout<<"return stbr\n";

            diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());
            diag_moves.insert(diag_moves.end(), scout_bl.begin(), scout_bl.end());

            //cout << "back from scouting";

            //cout << "0,2" << board[0][2] << endl;

            for (auto it : diag_moves)
            {
                //cout << "Loop 14" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;

                //cout << "ins_row : " << ins_row << endl;
                //cout << "ins_col : " << ins_col << endl;
                //cout << endl;

                board[ins_row][ins_col] = player_char;
                //cout << "board : " << board[ins_row][ins_col] << endl << endl;
            }
            //cout << "out of loop" << endl;
            //print_board_with_valid_moves();
            //cout << "retuning from the top diagonal box\n";
            return;
        }
        //return;

        //bottom side board case

        if ((input_row == 6 && input_column >= 2 && input_column <= 5) || (input_row == 7 && input_column >= 2 && input_column <= 5))
        {
            scout_tl = scout_towards_top_left(input);
            scout_tr = scout_towards_top_right(input);

            diag_moves.insert(diag_moves.end(), scout_tr.begin(), scout_tr.end());
            diag_moves.insert(diag_moves.end(), scout_tl.begin(), scout_tl.end());

            for (auto it : diag_moves)
            {
                //cout << "Loop 15" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the bottom diagonal box\n";
            return;
        }

        //left side board case

        if ((input_column == 0 && input_row >= 2 && input_row <= 5) || (input_column == 1 && input_row >= 2 && input_row <= 5))
        {
            scout_br = scout_towards_bottom_right(input);
            scout_tr = scout_towards_top_right(input);

            diag_moves.insert(diag_moves.end(), scout_tr.begin(), scout_tr.end());
            diag_moves.insert(diag_moves.end(), scout_br.begin(), scout_br.end());

            for (auto it : diag_moves)
            {
                //cout << "Loop 16" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the left diagonal box\n";
            return;
        }

        if ((input_column == 6 && input_row >= 2 && input_row <= 5) || (input_column == 7 && input_row >= 2 && input_row <= 5))
        {
            scout_bl = scout_towards_top_left(input);
            scout_tl = scout_towards_bottom_left(input);

            diag_moves.insert(diag_moves.end(), scout_tl.begin(), scout_tl.end());
            diag_moves.insert(diag_moves.end(), scout_bl.begin(), scout_bl.end());

            for (auto it : diag_moves)
            {
                //cout << "Loop 17" << endl;
                int ins_row = it / 10;
                int ins_col = it % 10;
                board[ins_row][ins_col] = player_char;
            }
            //cout << "retuning from the right diagonal box\n";
            return;
        }
        //cout<<"returning from flip tiles\n";
    }

}

int random_playouts(int valid_move)
{

    string random_backup_board[8][8];
    copy(&board[0][0], &board[0][0]+8*8,&random_backup_board[0][0]);

    board[valid_move/10][valid_move%10] = user;

    vector<int> random_moves = curr_valid_moves();
    int num_of_moves = random_moves.size();

    bool rand_human = 1;

    //srand(time(0));
    //cout<<"B2"<<endl;
    while(true)
    {
        if(random_moves.size() == 0)
        {
            return 1;
        }

        int turn = rand()%num_of_moves;
        //cout <<"Turn in random playouts: "<<turn<<endl;
        int move = random_moves[turn];
        //cout <<"Turn in random playouts: "<<move<<endl<<endl;

        if (rand_human == 1)
        {
            flip_tiles(move);
        }

        else
        {
            flip_tiles(move);
        }
        rand_human = !rand_human;

        if (get_winner() == ai)
        {
            copy(&random_backup_board[0][0], &random_backup_board[0][0]+8*8,&board[0][0]);
            return 2;
        }

        if(get_winner() == user)
        {
            copy(&random_backup_board[0][0], &random_backup_board[0][0]+8*8,&board[0][0]);
            return 0;
        }

        if(get_winner() == "tie")
        {
            copy(&random_backup_board[0][0], &random_backup_board[0][0]+8*8,&board[0][0]);
            return 1;
        }

        else
        {
            continue;
        }
    }
}

int random_playouts_heuristics(int valid_move)
{

    string random_backup_board[8][8];
    copy(&board[0][0], &board[0][0]+8*8,&random_backup_board[0][0]);

    board[valid_move/10][valid_move%10] = user;

    vector<int> random_moves = curr_valid_moves();
    int num_of_moves = random_moves.size();

    bool rand_human = 1;

    int default_score = 0;
    while(true)
    {
        if(random_moves.size() == 0)
        {
            return 1;
        }
        int turn = rand()%num_of_moves;
        int move;

        for(auto i : random_moves)
        {
            int i_row = i/10;
            int i_col = i%10;

            int num = num_of_moves;
            int available[8][8];

            for(int i = 0;i<8;i++)
            {
                for(int j = 0; j < 8; j++)
                {
                    if(random_moves[num] == (i*10+j))
                    {
                        move = random_moves[num];
                    }
                    num--;
                }
            }
        }
        //int move = random_moves[turn];

        if (rand_human == 1)
        {
            flip_tiles(move);
        }

        else
        {
            flip_tiles(move);
        }
        rand_human = !rand_human;

        if (get_winner() == ai)
        {
            copy(&random_backup_board[0][0], &random_backup_board[0][0]+8*8,&board[0][0]);
            return 2;
        }

        if(get_winner() == user)
        {
            copy(&random_backup_board[0][0], &random_backup_board[0][0]+8*8,&board[0][0]);
            return 0;
        }

        if(get_winner() == "tie")
        {
            copy(&random_backup_board[0][0], &random_backup_board[0][0]+8*8,&board[0][0]);
            return 1;
        }

        else
        {
            continue;
        }
    }
}


void mcts_heuristics()
{
    string heuristics_backup_board[8][8];//original board
    copy(&board[0][0], &board[0][0]+8*8,&heuristics_backup_board[0][0]);

    vector<int> valid_moves_here_heuristics = valid_moves();
    int num_valid_moves_heuristics = valid_moves_here_heuristics.size();

    vector<int> playout_score_heuristics;

    for (int i = 0 ; i < num_valid_moves_heuristics ; i++ )
    {
        int total = 0;
        for(int j = 0; j < 100 ; j++)//i is less than the number of playouts
        {
            //cout<<"i : "<<i<<"valid_moves: "<<valid_moves_here[i]<<endl;
            int result = random_playouts_heuristics(valid_moves_here_heuristics[i]);
            total += result;
        }
        playout_score_heuristics.push_back(total);
    }

    int maxindex = std::distance(playout_score_heuristics.begin(),std::max_element(playout_score_heuristics.begin(), playout_score_heuristics.end()));

    //cout<<"MAXINDEX in AI = "<<maxindex<<endl;

    copy(&heuristics_backup_board[0][0], &heuristics_backup_board[0][0]+8*8,&board[0][0]);
    //cout<<"user turn: " <<is_user_turn<<endl;

    valid_moves_here_heuristics = valid_moves();
    sort(valid_moves_here_heuristics.begin(), valid_moves_here_heuristics.end());
    //cout<<"valid_moves_here[maxindex]"<<valid_moves_here[maxindex]<<endl;
    //int num_valid_moves = valid_moves_here.size();
    cout << " AI chooses : " << valid_moves_here_heuristics[maxindex] << endl;
    flip_tiles (valid_moves_here_heuristics[maxindex]);
    return;
}

void mcts()
{
    //backup of the caller functions board
    string backup_board[8][8];//original board
    copy(&board[0][0], &board[0][0]+8*8,&backup_board[0][0]);


    //getting valid moves and the amount of them
    vector<int> valid_moves_here = valid_moves();
    int num_valid_moves = valid_moves_here.size();

    //to keep track of number of wins,losses and draws from each valid move
    vector<int>playout_score;
    for (int i = 0 ; i < num_valid_moves ; i++ )
    {
        int total = 0;
        for(int j = 0; j < 100 ; j++)//i is less than the number of playouts
        {
            int result = random_playouts(valid_moves_here[i]);
            total += result;
        }
        playout_score.push_back(total);
    }


    int maxindex = std::distance(playout_score.begin(),std::max_element(playout_score.begin(), playout_score.end()));


    copy(&backup_board[0][0], &backup_board[0][0]+8*8,&board[0][0]);


    valid_moves_here = valid_moves();
    sort(valid_moves_here.begin(), valid_moves_here.end());
    cout << " AI chooses : " << valid_moves_here[maxindex] << endl;
    flip_tiles (valid_moves_here[maxindex]);


}

//main gameplay

void game_play() {
    int input;
    while (game_still_running) {

        if(check_if_over())
        {
            print_winner();
            break;
        }

        if (previous_user_passed_turn)
        {
            is_user_turn  = !is_user_turn;
        }

        vector<int> current_valid_moves = valid_moves();
        sort(current_valid_moves.begin(), current_valid_moves.end());

        cout << "Pick a Valid Index" << endl;
        for (auto it : current_valid_moves)
        {
            cout << it << " ";
        }

        if (is_user_turn)
        {
            cout << "\nO Turn" << endl;
            cout << "\nChoose a place : ";
            cin >> input;
            std::vector<int>::iterator it;

            if (input == -1)
            {
                cout << "EXITING\n";
                return;
            }

            //it  = find(current_valid_moves.begin(),current_valid_moves.end(),input);
            while(true)
            {
                it  = find(current_valid_moves.begin(),current_valid_moves.end(),input);
                if(it != current_valid_moves.end())
                {
                    break;
                }

                else
                {
                    cout<<"Invalid Input! TRY AGAIN\n";
                    cout<<"Pick a value from the given VALID inputs: \n";
                    for (auto it : current_valid_moves)
                    {
                        cout << it << " ";
                    }

                    cout<<"\nInput >> ";
                    cin>>input;
                    cout<<endl;
                    if (input == -1)
                    {
                        cout << "EXITING\n";
                        return;
                    }
                }

            }
        }

        else
        {
            cout << "\nX Turn" << endl;
        }

        if (is_user_turn)
        {
            user_turn(input);
            is_user_turn = 0;
        }
        else
        {
            ai_turn();
            is_user_turn = 1;
        }
        print_board_with_valid_moves();

    }
}

void ai_game_play() {
    //int input;
    populate_score_board();
    int mcts_won = 0;
    int heuristics_won = 0;
    int tied = 0;

    for(int i = 0; i < 10; i++)
    {
        initialize_board();
        print_board_with_valid_moves();
        while (game_still_running) {

            vector<int> current_valid_moves = valid_moves();
            sort(current_valid_moves.begin(), current_valid_moves.end());

            if(check_if_over())
            {
                print_winner();
                previous_user_passed_turn = 0;
                break;
            }

            if (previous_user_passed_turn)
            {
                is_user_turn  = !is_user_turn;
            }

            cout << "Valid Index : " << endl;
            for (auto it : current_valid_moves)
            {
                cout << it << " ";
            }

            if (!is_user_turn)//change
            {
                mcts();
                is_user_turn = 1;//change
            }
            else
            {
                mcts_heuristics();
                is_user_turn = 0;//change
            }
            print_board_with_valid_moves();
        }
        if (get_winner() == ai)
        {
            mcts_won ++;
        }

        else if(get_winner() == user)
        {
            heuristics_won ++;

        }

        if(get_winner() == "tie")
        {
            tied ++;
        }
        game_still_running = 1;
        previous_user_passed_turn = 0;
    }

    cout << "MCTS won: " << mcts_won <<" times\n";
    cout << "Heuristics won: " << heuristics_won <<" times\n";
}

int main() {

    srand(time(0));


    cout<<"Instructions: Choose a correct place like 24 (2 for row and 4 for column)\n";

//comment out these three lines and uncomment ai_game_play() to see 2 AI competing.
    initialize_board();
    print_board_with_valid_moves();
    game_play();

//    ai_game_play();

    return 0;
}
