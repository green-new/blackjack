// blackjack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

enum suits {
    Hearts = 0x00,
    Diamonds = 0x10,
    Clubs = 0x20,
    Spades = 0x30
};
enum class state {
	Win,
	Lost,
	Push,
	Bust
};

const int suitmask = 0xF0;
const int numbermask = 0xF;

void init_deck(vector<unsigned int> & deck) {
    int suits[4] = { Hearts, Diamonds, Clubs, Spades };
    for (int s = 0; s < 4; s++) {
        for (int x = 0; x < 13; x++) {
            int card = suits[s] + (x + 1);
            deck.push_back(card);
        }
    }
}

string card_name(unsigned int card) {
	unsigned int suit = card & suitmask;
	unsigned int number = card & numbermask;

	string str_suit;
	string str_number;

	switch (suit) {
	case Hearts:
		str_suit = "Hearts";
		break;
	case Diamonds:
		str_suit = "Diamonds";
		break;
	case Clubs:
		str_suit = "Clubs";
		break;
	case Spades:
		str_suit = "Spades";
		break;
	}

	switch (number) {
	case 1:
		str_number = "Ace";
		break;
	case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10:
		str_number = to_string(number);
		break;
	case 11:
		str_number = "Jack";
		break;
	case 12:
		str_number = "Queen";
break;
	case 13:
		str_number = "King";
		break;
	}

	return str_number + " of " + str_suit;
}

// Fisher-Yates shuffling algorithm
void shuffle(vector<unsigned int>& deck) {
	for (int n = 52 - 1; n > 1; n--) {
		int j = rand() % n + 1;
		int buffer = deck[n];
		deck[n] = deck[j];
		deck[j] = buffer;
	}
}

unsigned int draw(vector<unsigned int>& deck) {
	int card = deck.back();
	deck.pop_back();

	return card;
}

void print_hand(vector<unsigned int> hand) {
	auto it = hand.begin();
	while (it != hand.end()) {
		cout << card_name(*it) << endl;
		it++;
	}
}

unsigned int hardsumofhand(vector<unsigned int> hand) {
	auto it = hand.begin();
	int hardsum = 0;
	while (it != hand.end()) {
		int number = *it & numbermask;
		switch (number) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			hardsum += number;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
			hardsum += 10;
			break;
		}

		it++;
	}
	return hardsum;
}

unsigned int softsumofhand(vector<unsigned int> hand) {
	auto it = hand.begin();
	int softsum = 0;
	while (it != hand.end()) {
		int number = *it & numbermask;
		switch (number) {
		case 1:
			softsum += 11;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			softsum += number;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
			softsum += 10;
			break;
		}

		it++;
	}
	return softsum;
}

bool check_bust(vector<unsigned int>& hand) {
	int total = hardsumofhand(hand);
	return total > 21;
}

void hit(vector<unsigned int>& deck, vector<unsigned int>& hand) {
	hand.push_back(draw(deck));
}

void stand(vector<unsigned int>& deck, vector<unsigned int>& dealerhand) {
	bool flag = false;
	while (!flag) {
		int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
		if (dealertotal <= 16) {
			hit(deck, dealerhand);
			flag = check_bust(dealerhand);
		}
		if (dealertotal >= 17) {
			flag = true;
			return;
		}
	}
}

void doubledown(vector<unsigned int>& deck, vector<unsigned int>& playerhand, int & bet) {
	bet *= 2;
	return hit(deck, playerhand);
}

void split(vector<unsigned int>& deck, vector<unsigned int>& playerhand, vector<unsigned int> & splithand, int & bet) {
	vector<unsigned int> buffer = playerhand;
	playerhand.clear();
	playerhand.push_back(buffer.back());
	buffer.pop_back();

	splithand.clear();
	splithand.push_back(buffer.back());
	buffer.pop_back();

	bet *= 2;
}

enum class playermove {
	Stand,
	Hit,
	Double,
	Split
};

playermove player_input(bool cansplit) {
	bool flag = true;
	string input;

	do {
		if (!cansplit) {
			cout << endl << "Z to stand, X to hit, C to double-down." << endl;
		} else {
			cout << endl << "Z to stand, X to hit, C to double-down, S to split hand." << endl;
		}
		cin >> input;
		if (input == "Z") {
			return playermove::Stand;
		}
		else if (input == "X") {
			return playermove::Hit;
		}
		else if (input == "C") {
			return playermove::Double;
		}
		else if (input == "S" && cansplit == true) {
			return playermove::Split;
		}
		else {
			cout << "You've provided invalid input. Try again." << endl;
		}
	} while (flag);
	cout << endl;
}

state check_blackjack(vector<unsigned int>& playerhand, vector<unsigned int>& dealerhand) {
	int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
	int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
	if (playertotal == dealertotal) {
		return state::Push;
	}
	else if (playertotal == 21) {
		return state::Win;
	}
	else if (dealertotal == 21) {
		return state::Lost;
	}
}

state get_state(vector<unsigned int>& playerhand, vector<unsigned int>& dealerhand) {
	int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
	int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);

	if (playertotal > 21) {
		return state::Bust;
	}
	if ((playertotal > dealertotal || dealertotal > 21) && playertotal <= 21) {
		return state::Win;
	}
	if ((playertotal < dealertotal || playertotal > 21) && dealertotal <= 21) {
		return state::Lost;
	}
	if (playertotal == dealertotal) {
		return state::Push;
	}
}

state check_win(vector<unsigned int>& playerhand, vector<unsigned int>& dealerhand) {
	int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
	int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);

	state finalstate = get_state(playerhand, dealerhand);

	if (finalstate == state::Win) {
		cout << "======You won!======" << endl;
		cout << "Your hand was:" << endl;
		print_hand(playerhand);
		int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
		cout << "(" << playertotal << ")" << endl << endl;
		cout << "The dealer's hand was:" << endl;
		print_hand(dealerhand);
		int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
		cout << "(" << dealertotal << ")" << endl;
		cout << "====================" << endl;
		return state::Win;
	}
	if (finalstate == state::Lost) {
		cout << "======You lost.======" << endl;
		cout << "Your hand was:" << endl;
		print_hand(playerhand);
		int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
		cout << "(" << playertotal << ")" << endl << endl;
		cout << "The dealer's hand was:" << endl;
		print_hand(dealerhand);
		int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
		cout << "(" << dealertotal << ")" << endl;
		cout << "=====================" << endl;
		return state::Lost;
	}
	if (finalstate == state::Bust) {
		cout << "======You bust!======" << endl;
		cout << "Your hand was:" << endl;
		print_hand(playerhand);
		int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
		cout << "(" << playertotal << ")" << endl << endl;
		cout << "The dealer's hand was:" << endl;
		print_hand(dealerhand);
		int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
		cout << "(" << dealertotal << ")" << endl;
		cout << "=====================" << endl;
		return state::Bust;
	}
	if (finalstate == state::Push) {
		cout << "=====Its a push.=====" << endl;
		cout << "Your hand was:" << endl;
		print_hand(playerhand);
		int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
		cout << "(" << playertotal << ")" << endl << endl;
		cout << "The dealer's hand was:" << endl;
		print_hand(dealerhand);
		int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
		cout << "(" << dealertotal << ")" << endl;
		cout << "=====================" << endl;
		return state::Push;
	}
}

int bet_input(int equity) {
	string input;
	cout << "How much are you betting?" << endl;
	cin >> input;
	int bet = 100;
	do {
		try {
			bet = stoi(input);
			if (input == "") {
				bet = 100;
			}
		}
		catch (const invalid_argument e) {
			bet = 100;
		}
		catch (const out_of_range e) {
			bet = 100;
		}
	} while (bet < equity && bet > equity);

	cout << "Bet is $" << bet << "." << endl;

	return bet;
}
int main() {
    srand(time(NULL));
    vector<unsigned int> deck;
	vector<unsigned int> playerhand;
	vector<unsigned int> splithand1; // You can split up to once
	vector<unsigned int> dealerhand;

    init_deck(deck);

    shuffle(deck);

	int equity = 10000;
	int wins = 0;
	int games = 0;
	float winrate = 0.0f;
	while (true) {
		int bet;
		cout << "Equity: $" << equity << endl;
		if (games > 0) {
			winrate = (float)wins / (float)games;
			cout << wins << "/" << games << " (" << winrate << " winrate)" << endl;
		}
		if (equity < 0) {
			cout << "You have no money left! You lose!" << endl;
			return 0;
		}

		if (deck.size() <= 12) {
			cout << "Shuffling deck...";
			init_deck(deck);
			shuffle(deck);
		}

		playerhand.clear();
		splithand1.clear();
		dealerhand.clear();

		bet = bet_input(equity);

		// Deal players hand
		playerhand.push_back(draw(deck));
		playerhand.push_back(draw(deck));

		// Deal dealers hand
		dealerhand.push_back(draw(deck));
		dealerhand.push_back(draw(deck));

		cout << "Your hand is: \n";
		print_hand(playerhand);
		int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
		cout << "(" << playertotal << ")" << endl << endl;
		
		cout << "The dealer's upcard is: " << endl << card_name(dealerhand.back()) << endl << endl;

		state blackjack = check_blackjack(playerhand, dealerhand);
		if (blackjack == state::Win) {
			cout << "=====Blackjack! You win!=====" << endl;
			cout << "Your hand was:" << endl;
			print_hand(playerhand);
			int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
			cout << "(" << playertotal << ")" << endl << endl;
			cout << "The dealer's hand was:" << endl;
			print_hand(dealerhand);
			int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
			cout << "(" << dealertotal << ")" << endl;
			cout << "=====================" << endl;
			wins++;
			games++;
			equity += bet + bet;
			continue;
		}
		else if (blackjack == state::Lost) {
			cout << "=====Blackjack... You lose.=====" << endl;
			cout << "Your hand was:" << endl;
			print_hand(playerhand);
			int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
			cout << "(" << playertotal << ")" << endl << endl;
			cout << "The dealer's hand was:" << endl;
			print_hand(dealerhand);
			int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
			cout << "(" << dealertotal << ")" << endl;
			cout << "=====================" << endl;
			games++;
			equity -= bet;
			continue;
		}
		else if (blackjack == state::Push) {
			cout << "=====Blackjack... For both. Push.=====" << endl;
			cout << "Your hand was:" << endl;
			print_hand(playerhand);
			int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
			cout << "(" << playertotal << ")" << endl << endl;
			cout << "The dealer's hand was:" << endl;
			print_hand(dealerhand);
			int dealertotal = (softsumofhand(dealerhand) > 21) ? hardsumofhand(dealerhand) : softsumofhand(dealerhand);
			cout << "(" << dealertotal << ")" << endl;
			cout << "=====================" << endl;
			games++;
			continue;
		}

		bool inputflag = true;
		bool split1flag = true;
		bool split2flag = true;

		while (inputflag) {
			playermove move;
			move = player_input((playerhand.size() == 2) && (playerhand.at(0) & numbermask) == (playerhand.at(1) & numbermask));

			if (move == playermove::Stand) {
				stand(deck, dealerhand);
				inputflag = false;
			}
			else if (move == playermove::Hit) {
				hit(deck, playerhand);
			}
			else if (move == playermove::Double) {
				doubledown(deck, playerhand, bet);
			}
			else if (move == playermove::Split) {
				split(deck, playerhand, splithand1, bet);

				while (split1flag) {
					playermove move;
					move = player_input(false);

					if (move == playermove::Stand) {
						stand(deck, dealerhand);
						split1flag = false;
					}
					else if (move == playermove::Hit) {
						hit(deck, playerhand);
					}
					else if (move == playermove::Double) {
						doubledown(deck, playerhand, bet);
					}
					if (check_bust(playerhand)) {
						break;
					}
					if (check_bust(dealerhand)) {
						break;
					}
					cout << "Your hand is now: \n";
					print_hand(playerhand); 
					int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
					cout << "(" << playertotal << ")" << endl << endl;
				}

				games++;

				state splitstate = check_win(playerhand, dealerhand);
				if (splitstate == state::Win) {
					equity += bet + bet;
					wins++;
				}
				else if (splitstate == state::Lost || splitstate == state::Bust) {
					equity -= bet;
				}

				while (split2flag) {
					playermove move;
					move = player_input(false);

					if (move == playermove::Stand) {
						stand(deck, dealerhand);
						split2flag = false;
					}
					else if (move == playermove::Hit) {
						hit(deck, splithand1);
						cout << "Your hand is now: \n";
						print_hand(splithand1);
						int playertotal = (softsumofhand(splithand1) > 21) ? hardsumofhand(splithand1) : softsumofhand(splithand1);
						cout << "(" << playertotal << ")" << endl << endl;
					}
					else if (move == playermove::Double) {
						doubledown(deck, splithand1, bet);
						cout << "Your hand is now: \n";
						print_hand(splithand1);
					}
					if (check_bust(splithand1)) {
						break;
					}
					if (check_bust(dealerhand)) {
						break;
					}

					cout << "Your hand is now: \n";
					print_hand(splithand1);
					int playertotal = (softsumofhand(splithand1) > 21) ? hardsumofhand(splithand1) : softsumofhand(splithand1);
					cout << "(" << playertotal << ")" << endl << endl;
				}

				games++;

				state splitstate1 = check_win(splithand1, dealerhand);
				if (splitstate1 == state::Win) {
					equity += bet + bet;
					wins++;
				}
				else if (splitstate1 == state::Lost || splitstate1 == state::Bust) {
					equity -= bet;
				}
			}

			if (check_bust(playerhand)) {
				break;
			}
			if (check_bust(dealerhand)) {
				break;
			}

			cout << "Your hand is now: \n";
			print_hand(playerhand);
			int playertotal = (softsumofhand(playerhand) > 21) ? hardsumofhand(playerhand) : softsumofhand(playerhand);
			cout << "(" << playertotal << ")" << endl << endl;
		}

		state finalstate = check_win(playerhand, dealerhand);
		if (finalstate == state::Win) {
			equity += bet + bet;
			wins++;
		}
		else if (finalstate == state::Lost || finalstate == state::Bust) {
			equity -= bet;
		}
		games++;
	}

    return 0;
}
