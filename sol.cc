
#include <algorithm>
#include <iostream>
#include <memory>
#include <map>
#include <unordered_set>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

#include <stdlib.h>
#include <stdio.h>

using namespace std;

class Deck {
 public:
  Deck() {
    unused_cards_.reserve(40);
    used_cards_.reserve(40);
    for (int i = 0; i < 10; ++i) {
      unused_cards_.push_back(i);
      unused_cards_.push_back(i);
      unused_cards_.push_back(i);
      unused_cards_.push_back(i);
    }
    Shuffle();
  }
  
  void Shuffle() {
    srand(time(0));
    for (int i = unused_cards_.size(); i > 1; --i) {
      int r = rand() % i;
      int temp = unused_cards_[i-1];
      unused_cards_[i-1] = unused_cards_[r];
      unused_cards_[r] = temp;
    }
  }

  void ClearUsed() {
    used_cards_.clear();
  }

  int GetCard() {
    if (unused_cards_.empty()) {
      return -1;
    }
    int output = unused_cards_.back();
    unused_cards_.pop_back();
    used_cards_.push_back(output);
    return output;
  }

  bool HasCard() const {
    return !unused_cards_.empty();
  }

  int Top() const {
    if (used_cards_.empty()) {
      return -1;
    }
    return used_cards_.back();
  }

  void UseTop() {
    return used_cards_.pop_back();
  }

  void Print() const {
    for (int i = 0; i < used_cards_.size(); ++i) {
      cout << used_cards_[i];
    }
    cout << "  ";
    for (int i = 0; i < unused_cards_.size(); ++i) {
      cout << unused_cards_[unused_cards_.size() - i - 1];
    }
    cout << endl;
  }

  vector<int> unused_cards_;
  vector<int> used_cards_;
};

class Board {
 public:
  Board() {
    all_cards_.reserve(kLevels*(kLevels+1)/2);
    for (int k = 0; k < kLevels; ++k) {
      for (int i = 0; i < k+1; ++i) {
        all_cards_.push_back(deck_.GetCard());
      }
    }
    deck_.ClearUsed();
    ClearKings();
  }

  bool Won() const {
    return all_cards_[0] == -1;
  }

  void ClearKings() {
    bool again = false;
    for (int k = 0; k < kLevels; ++k) {
      for (int i = 0; i < k+1; ++i) {
        if (Uncovered(k, i) && Card(k, i) == 0) {
	  again = true;
          SetCard(k, i, -1);
        }
      }
    }
    if (again) {
      // Make sure we clear cascading kings.
      ClearKings();
    }
  }

  int Card(const int level, const int card) const {
    int index = level*(level+1)/2 + card;
    if (index < 0 || index >= all_cards_.size()) {
      return -1;
    }
    return all_cards_[index];
  }

  void SetCard(const int level, const int card, const int value) {
    int index = level*(level+1)/2 + card;
    if (index < 0 || index >= all_cards_.size()) {
      return;
    }
    all_cards_[index] = value;
  }

  bool Uncovered(const int level, const int card) const {
    return (Card(level+1, card) == -1 && Card(level+1, card+1) == -1);
  }

  bool operator<(const Board& other) const {
    if (deck_.used_cards_.size() != other.deck_.used_cards_.size()) {
      return deck_.used_cards_.size() < other.deck_.used_cards_.size();
    }
    if (deck_.unused_cards_.size() != other.deck_.unused_cards_.size()) {
      return deck_.unused_cards_.size() < other.deck_.unused_cards_.size();
    }
    if (all_cards_.size() != other.all_cards_.size()) {
      return all_cards_.size() < other.all_cards_.size();
    }

    for (int i = 0; i < all_cards_.size(); ++i) {
      if (all_cards_[i] < other.all_cards_[i]) {
        return all_cards_[i] < other.all_cards_[i];
      }
    }
    for (int i = 0; i < deck_.unused_cards_.size(); ++i) {
      if (deck_.unused_cards_[i] < other.deck_.unused_cards_[i]) {
        return deck_.unused_cards_[i] < other.deck_.unused_cards_[i];
      }
    }
    for (int i = 0; i < deck_.used_cards_.size(); ++i) {
      if (deck_.used_cards_[i] < other.deck_.used_cards_[i]) {
        return deck_.used_cards_[i] < other.deck_.used_cards_[i];
      }
    }
    return false;
  }

  void Print() const {
    int current = 0;
    for (int k = 0; k < kLevels; ++k) {
      for (int i = 0; i < kLevels-k-1; ++i) {
        cout << "  ";
      }
      for (int i = 0; i < k+1; ++i) {
        if (all_cards_[current] == -1) {
          cout << "    ";
        } else {
          if (Uncovered(k, i)) {
            cout << " " << all_cards_[current] << "  ";
          } else {
            cout << "*" << all_cards_[current] << "* ";
          }
        }
        ++current;
      }
      cout << endl;
    }
    cout << endl;
    deck_.Print();
    cout << endl;
  }

  vector<Board> PossibleMoves() const {
    vector<pair<int, int>> playable;
    int current = 0;
    for (int k = 0; k < kLevels; ++k) {
      for (int i = 0; i < k+1; ++i) {
        if (all_cards_[current] != -1 && Uncovered(k, i)) {
          playable.push_back(pair<int, int>(all_cards_[current], current));
        }
        ++current;
      }
    }
    if (deck_.Top() != -1) {
      playable.push_back(pair<int, int>(deck_.Top(), -1));
    }

    vector<Board> boards;
    for (int i = 0; i < playable.size(); ++i) {
      for (int k = i+1; k < playable.size(); ++k) {
        if (playable[i].first + playable[k].first == 10) {
	  boards.emplace_back(*this);
	  if (playable[i].second == -1) {
	    boards.back().deck_.UseTop();
	  } else {
	    boards.back().all_cards_[playable[i].second] = -1;
	  }
	  if (playable[k].second == -1) {
	    boards.back().deck_.UseTop();
	  } else {
	    boards.back().all_cards_[playable[k].second] = -1;
	  }
	  boards.back().ClearKings();
	}
      }
    }
    if (deck_.HasCard()) {
      boards.emplace_back(*this);
      boards.back().deck_.GetCard();
      while (boards.back().deck_.Top() == 0) {
        boards.back().deck_.UseTop();
        boards.back().deck_.GetCard();
      }
    }

    return boards;
  }

 private:
  static const int kLevels;
  Deck deck_;
  vector<int> all_cards_;
};

const int Board::kLevels = 6;

set<Board> seen_boards;

bool Play(const Board& orig) {
  orig.Print();
  if (orig.Won()) {
    return true;
  }
  vector<Board> boards = orig.PossibleMoves();
  bool won = false;
  set<int> skip;
  for (int i = 0; i < boards.size(); ++i) {
    if (seen_boards.find(boards[i]) != seen_boards.end()) {
      skip.insert(i);
      continue;
    }
    seen_boards.emplace(boards[i]);
  }
  for (int i = 0; i < boards.size(); ++i) {
    if (skip.find(i) != skip.end()) {
      continue;
    }
    if (Play(boards[i])) {
      boards[i].Print();
      return true;
    }
  }
  return won;
}

int main() {
  Board orig;
  Play(orig);
  return 0;
}
