#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>  // For shuffle
#include <random>     // For random engine

using namespace std;

// Card structure
struct Card {
    string color;
    int number;  // Used for number cards
    string type; // Used for special cards (e.g., skip, reverse, wild, wildDraw4)

    // Default constructor for empty Card (required for special cards)
    Card() : color(""), number(-1), type("") {}

    // Constructor for number cards
    Card(string c, int n) : color(c), number(n), type("") {}

    // Constructor for wild cards
    Card(string t) : color(""), number(-1), type(t) {}

    // Constructor for special cards
    Card(string c,string t) : color(c), number(-1), type(t) {}
    void printCard() const {
        if (type == "wild" || type == "wildDraw4") {
            cout << type<<endl;
        }
        else if (number == -1)
        {
            cout<<color<<" (" << type<<")"<<endl;
        }
        else
        {
            cout << color << " (" << number << ")"<<endl;
        }
    }
};

// Deck class
class Deck {
public:
    vector<Card> cards;

    Deck() {
        string colors[] = { "Red", "Green", "Blue", "Yellow" };
        // Adding number cards (0-9)
        for (const string& color : colors) {
            for (int i = 0; i < 10; ++i) {
                cards.push_back(Card(color, i));
            }
        }

        // Adding special cards (skip, reverse, draw2) for each color
        for (const string& color : colors) {
            for (int i = 0; i < 2; ++i) {  // Each color has 2 special cards
                cards.push_back(Card(color,"skip"));
                cards.push_back(Card(color,"reverse"));
                cards.push_back(Card(color,"draw2"));
            }
        }

        // Adding wild cards
        for (int i = 0; i < 4; ++i) {
            cards.push_back(Card("wild"));
            cards.push_back(Card("wildDraw4"));
        }

        // Shuffle the deck
        srand(static_cast<unsigned int>(time(0)));  // Seed for random number generator
        shuffle(cards.begin(), cards.end(), default_random_engine(static_cast<unsigned int>(time(0))));
    }

    Card drawCard() {
        if (cards.empty()) {
            cout << "Deck is empty!" << endl;
            exit(1);
        }
        Card card = cards.back();
        cards.pop_back();
        return card;
    }
};

// Player class
class Player {
public:
    string name;
    vector<Card> hand;

    Player(string n) : name(n) {}

    void addCard(Card c) {
        hand.push_back(c);
    }

    void removeCard(int index) {
        if (index >= 0 && index < hand.size()) {
            hand.erase(hand.begin() + index);
        }
    }

    void printHand() const {
        for (int i = 0; i < hand.size(); ++i) {
            cout << i + 1 << ". ";
            hand[i].printCard();
            //cout << "  ";
        }
        cout << endl;
    }

    bool canPlay(const Card& topCard, const Card& card) {
        // Wild cards and Wild Draw Four can always be played
        if (card.type == "wild" || card.type == "wildDraw4") {
            return true;
        }

        // Card can be played if it matches the top card color or number (if it's a number card)
        if (card.color == topCard.color || card.number == topCard.number || card.type == topCard.type) {
            return true;
        }

        return false;
    }
};

// Main Game Class
class UnoGame {
private:
    vector<Player> players;
    Deck deck;
    Card topCard;
    int currentPlayer;
    bool reverseDirection;
    bool skipNextPlayer;
    bool gameOver;

public:
    UnoGame(int numPlayers) {
        reverseDirection = false;
        skipNextPlayer = false;
        gameOver = false;
        currentPlayer = 0;

        // Create players (including the user)
        for (int i = 0; i < numPlayers; ++i) {
            if (i == 0) {
                players.push_back(Player("zain"));
            }
            else {
                players.push_back(Player("AI " + to_string(i)));
            }
        }

        // Deal cards to players
        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < players.size(); ++j) {
                players[j].addCard(deck.drawCard());
            }
        }

        // Start with a random top card
        topCard = deck.drawCard();
    }

    void nextPlayer() {
        // Check if the direction of play is reversed
        if (reverseDirection) {
            currentPlayer--;
            if (currentPlayer < 0) {
                currentPlayer = players.size() - 1; // Wrap around
            }
        }
        else {
            currentPlayer++;
            if (currentPlayer >= players.size()) {
                currentPlayer = 0; // Wrap around
            }
        }

        // Skip the next player if needed
        if (skipNextPlayer) {
            skipNextPlayer = false;
            nextPlayer();  // Skip this player's turn
        }
    }

    void handleSpecialCards(const Card& card) {
        if (card.type == "reverse") {
            reverseDirection = !reverseDirection;
        }
        else if (card.type == "skip") {
            skipNextPlayer = true;
        }
        else if (card.type == "draw2") {
            int next = (currentPlayer + (reverseDirection ? -1 : 1)) % players.size();
            for (int i = 0; i < 2; ++i) {
                players[next].addCard(deck.drawCard());
            }
            nextPlayer(); // Skip the next player
        }
        else if (card.type == "wildDraw4") {
            int next = (currentPlayer + (reverseDirection ? -1 : 1)) % players.size();
            for (int i = 0; i < 4; ++i) {
                players[next].addCard(deck.drawCard());
            }
        }
    }

    void playTurn() {
        Player& player = players[currentPlayer];

        if (skipNextPlayer) {
            cout << "Skipping " << player.name << "'s turn.\n";
            skipNextPlayer = false;
            nextPlayer();
            return;
        }

        if (player.name == "AI " + to_string(currentPlayer + 1)) {
            // AI plays a card
            cout << "AI is thinking...\n";
            bool played = false;
            for (int i = 0; i < player.hand.size(); ++i) {
                if (player.canPlay(topCard, player.hand[i])) {
                    topCard = player.hand[i];
                    player.removeCard(i);
                    played = true;
                    cout << "AI plays: ";
                    topCard.printCard();
                    cout << endl;
                    break;
                }
            }

            if (!played) {
                // AI has no valid card, so draw a card
                player.addCard(deck.drawCard());
            }
        }
        else {
            // Player's turn
            int cardChoice;
            cout << "Top card: ";
            topCard.printCard();
            cout << endl;
            if(topCard.type=="skip"){
				nextPlayer();
			}
            if (skipNextPlayer) {
                cout << "Skipping " << player.name << "'s turn.\n";
                skipNextPlayer = false;
                nextPlayer();
                return;
            }
            if (reverseDirection) {
                nextPlayer();
            }
            cout << player.name << "'s turn.\n";
            cout << "Your hand: " <<endl;
            player.printHand();

            cout << "Enter the card number to play or 0 to draw a card: ";
            cin >> cardChoice;

            if (cardChoice == 0) {
                // Draw a card
                player.addCard(deck.drawCard());
            }
            else if (cardChoice > 0 && cardChoice <= player.hand.size() && player.canPlay(topCard, player.hand[cardChoice - 1])) {
                topCard = player.hand[cardChoice - 1];
                player.removeCard(cardChoice - 1);
                handleSpecialCards(topCard);
            }
            else {
                cout << "Invalid move! The card doesn't match the top card.\n";
                return;
            }
        }

        // Check if the current player has won
        if (player.hand.empty()) {
            cout << player.name << " wins!" << endl;
            gameOver = true;
        }

        // Move to the next player
        nextPlayer();
    }

    void play() {
        while (!gameOver) {
            playTurn();
        }
    }
};

int main() {
    int numPlayers;
    cout << "Enter the number of players (2 to 10): ";
    cin >> numPlayers;

    if (numPlayers < 2 || numPlayers > 10) {
        cout << "Invalid number of players!" << endl;
        return 1;
    }

    UnoGame game(numPlayers);
    game.play();

    return 0;
}
