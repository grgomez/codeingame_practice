#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum search {
    COLDER,
    WARMER,
    SAME,
    UNKNOWN
};

int main()
{
    int W; // width of the building.
    int H; // height of the building.
    cin >> W >> H; cin.ignore();
    int N; // maximum number of turns before game over.
    cin >> N; cin.ignore();a
    int X0;
    int Y0;
    cin >> X0 >> Y0; cin.ignore();

    // game loop
    while (1) {
        string bombDir; // Current distance to the bomb compared to previous distance (COLDER, WARMER, SAME or UNKNOWN)
        cin >> bombDir; cin.ignore();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "0 0" << endl;
    }
}