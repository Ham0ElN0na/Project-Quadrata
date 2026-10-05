#include <iostream>
#include <string>

using namespace std;
class Review {
private:
    int starRating;

public:
    Review() : starRating(0) {}

    void inputRating() {
        int userChoice;

        while (true) {
            cout << "Enter your rating (1 to 5 Stars): ";
            cin >> userChoice;

            if (userChoice >= 1 && userChoice <= 5) {
                starRating = userChoice;
                break;
            }
            else {
                cout << "❌ Invalid input! Please choose a number between 1 and 5.\n";
            }
        }
    }

    void displayRatingWithEmoji() const {
        cout << "Rating: ";
        for (int i = 0; i < starRating; ++i) {
            cout << "⭐";
        }
        cout << " (" << starRating << "/5) ";

        switch (starRating) {
        case 1:
            cout << "😡 [Terrible]";
            break;
        case 2:
            cout << "😞 [Bad]";
            break;
        case 3:
            cout << "😐 [Good]";
            break;
        case 4:
            cout << "🙂 [Very Good]";
            break;
        case 5:
            cout << "🤩 [Excellent!]";
            break;
        }
        cout << "\n";
    }

    void displayReview() const {
        std::cout << "\n--- Your Review ---\n";
        displayRatingWithEmoji();
        std::cout << "-------------------\n";
    }
};

//int main() {
//#ifdef _WIN32
//    system("chcp 65001 > nul");
//#endif
//
//    Review userReview;
//
//    std::cout << "=== Welcome to the Booking Review System ===\n";
//    userReview.inputRating();
//    userReview.displayReview();
//
//    return 0;
//}