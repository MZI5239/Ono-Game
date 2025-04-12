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
    Card(int n,string t) : color(""), number(n), type(t) {}

    // Constructor for special cards
    Card(string c,int n,string t) : color(c), number(n), type(t) {}
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
                cards.push_back(Card(color,11,"skip"));
                cards.push_back(Card(color,12,"reverse"));
                cards.push_back(Card(color,13,"draw2"));
            }
        }

        // Adding wild cards
        for (int i = 0; i < 4; ++i) {
            cards.push_back(Card(14,"wild"));
            cards.push_back(Card(15,"wildDraw4"));
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

    bool canPlay(const Card& topCard, const Card& card, string forcedColor,int pendingDrawCards) {
        // Wild cards can always be played
        if ((card.type == "wildDraw4"&&topCard.type!= "wild")&& topCard.type != "draw2") {
            return true;
        }
        if((card.type == "wild"&& topCard.type !="wildDraw4")&& topCard.type!="draw2") {
			return true;
		}
        // If color is forced (from previous wild card)
        if ((topCard.type=="wildDraw4"|| topCard.type == "wild")&& pendingDrawCards==0) {
            return card.color == forcedColor;
        }
        if(card.type == "draw2" && topCard.type=="draw2") {
			return true;
		}
        if (card.type == "skip" && topCard.type == "skip") {
            return true;
        }
        if (card.type == "reverse" && topCard.type == "reverse") {
            return true;
        }
        // Normal play rules
        return ((card.color == topCard.color || card.number == topCard.number));
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
    int pendingDrawCards = 0;  // Tracks accumulated draw cards in a chain
    string pendingDrawType ; // Tracks the type of draw card in the chain
    string forcedColor;
    //bool colorForced = false;
public:
    UnoGame(int numPlayers,string name) {
        reverseDirection = false;
        skipNextPlayer = false;
        gameOver = false;
        currentPlayer = 0;
        pendingDrawType = "";
        forcedColor = "";
        // Create players (including the user)
        for (int i = 0; i < numPlayers; ++i) {
            if (i == 0) {
                players.push_back(Player(name));
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
        while (topCard.type == "wildDraw4" || topCard.type == "wild") {
            // If the first card is a wildDraw4, put it back and draw a new one
            deck.cards.push_back(topCard); // Put it back in the deck
            topCard = deck.drawCard(); // Add to existing chain
        }
        if (topCard.type == "draw2") {
            pendingDrawType = "draw2";
            pendingDrawCards += 2; // Add to existing chain
        }
        else if (topCard.type == "skip")
        {
            skipNextPlayer = true;
        }
		else if (topCard.type == "reverse")
		{
			reverseDirection = !reverseDirection;
		}
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
        else if (card.type == "wild" || card.type == "wildDraw4") {
            // Player chooses color
            if (1) {
                cout << "Choose a color (Red, Green, Blue, Yellow): ";
                cin >> forcedColor;
            }
            else {
                // Simple AI color choice - can be improved later
                string colors[] = { "Red", "Green", "Blue", "Yellow" };
                forcedColor = colors[rand() % 4];
                cout << "AI chose color: " << forcedColor << endl;
            }
            //colorForced = true;

            if (card.type == "wildDraw4") {
                pendingDrawCards += 4;
                pendingDrawType = "wildDraw4";
            }
        }
		else if (card.type == "draw2") {
			pendingDrawType = "draw2";
			pendingDrawCards += 2; // Add to existing chain
		}
        // ... rest of handleSpecialCards
    }

    void playTurn() {
        Player& player = players[currentPlayer];

        string currentForcedColor = forcedColor;

        if (skipNextPlayer) {
            cout << "Skipping " << player.name << "'s turn.\n";
            skipNextPlayer = false;
            nextPlayer();
            return;
        }

        int cardChoice;
        cout << "Top card: ";
        topCard.printCard();
        cout << endl;

        // Show pending draws if any
        if (pendingDrawCards > 0) {
            cout << "PENDING TO DRAW: " << pendingDrawCards << " cards!\n";
        }

        cout << player.name << "'s turn.\n";
        cout << "Your hand: " << endl;
        player.printHand();

        if (pendingDrawCards > 0) {
            cout << "You must play a " << pendingDrawType << " card or draw " << pendingDrawCards << " cards.\n";
            cout << "Enter the number of a " << pendingDrawType << " card to play, or 0 to draw: ";
            cin >> cardChoice;

            if (cardChoice == 0) {
                // Draw the pending cards
                for (int i = 0; i < pendingDrawCards; ++i) {
                    player.addCard(deck.drawCard());
                }
                pendingDrawCards = 0;
                
                if(pendingDrawType== "wildDraw4") {
                    forcedColor = currentForcedColor;
                    cout<<"Color forced: "<<forcedColor<<endl;
				}
                pendingDrawType = "";
            }
            else if (cardChoice > 0 && cardChoice <= player.hand.size() && player.canPlay(topCard, player.hand[cardChoice - 1], forcedColor,pendingDrawCards)) {
                topCard = player.hand[cardChoice - 1];
                player.removeCard(cardChoice - 1);

                // Handle card accumulation here
                if (topCard.type == "draw2" && pendingDrawType == "draw2") {
                    pendingDrawCards += 2; // Add to existing chain
                }
                else if (topCard.type == "wildDraw4" && pendingDrawType == "wildDraw4") {
                    pendingDrawCards += 4; // Add to existing chain
                }
                else if (topCard.type == "draw2" || topCard.type == "wildDraw4") {
                    // Starting a new chain
                    pendingDrawCards = (topCard.type == "draw2") ? 2 : 4;
                }

                //handleSpecialCards(topCard);

                // ... [rest of the code]
            }
            else {
                cout << "Invalid move! You must play a " << pendingDrawType << " card or draw.\n";
                return;
            }
        }
        else {
            // Normal player turn
            cout << "Enter the card number to play or 0 to draw a card: ";
            cin >> cardChoice;

            if (cardChoice == 0) {
                player.addCard(deck.drawCard());
            }
            else if (cardChoice > 0 && cardChoice <= player.hand.size() && player.canPlay(topCard, player.hand[cardChoice - 1], currentForcedColor,pendingDrawCards)) {
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
            return;
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
    string name;
    cout << "Enter your name: ";
    cin >> name;
    if (numPlayers < 2 || numPlayers > 10) {
        cout << "Invalid number of players!" << endl;
        return 1;
    }

    UnoGame game(numPlayers,name);
    game.play();

    return 0;
}
