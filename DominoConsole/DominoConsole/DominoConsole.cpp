

// Перший програмний проект на С++, номер варіанту 8, гра про доміно//

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <algorithm>
#include <random>
#include <optional>
#include <iomanip>
#include <numeric>

using namespace std;
// =========================================================
// КЛАС КІСТКИ ДОМИНО
// =========================================================

class Domino {
public:
    int left;
    int right;

    Domino(int l = 0, int r = 0) : left(l), right(r) {}

    // Повертає візуальне представлення однієї половини кістки
    vector <string> get_half(int number) const {
        switch (number) {
        case 0: return { "       ", "       ", "       " };
        case 1: return { "       ", "   o   ", "       " };
        case 2: return { "o      ", "       ", "      o" };
        case 3: return { "o      ", "   o   ", "      o" };
        case 4: return { "o     o", "       ", "o     o" };
        case 5: return { "o     o", "   o   ", "o     o" };
        case 6: return { "o     o", "o     o", "o     o" };
        default: return { "       ", "       ", "       " };
        }
    }

    // Повертає всю кістку у вигляді списку рядків
        vector<string> get_lines() const {
        auto top = get_half(left);
        auto bottom = get_half(right);

        return {
            "+-------+",
            "|" + top[0] + "|",
            "|" + top[1] + "|",
            "|" + top[2] + "|",
            "|-------|",
            "|" + bottom[0] + "|",
            "|" + bottom[1] + "|",
            "|" + bottom[2] + "|",
            "+-------+"
        };
    }
};


// =========================================================
// КЛАС-РОЗДАВАЧ
// =========================================================

class DominoDealer {
private:
        vector<Domino> dominoes;
        mt19937 rng;

public:
    DominoDealer() : rng(std::random_device{}()) {
        create_dominoes();
    }

    // Створення повного набору доміно
    void create_dominoes() {
        dominoes.clear();
        for (int i = 0; i <= 6; ++i) {
            for (int j = i; j <= 6; ++j) {
                dominoes.emplace_back(i, j);
            }
        }
    }

    // Перемішування кісток
    void shuffle() {
        std::shuffle(dominoes.begin(), dominoes.end(), rng);
    }

    // Оператор ()
    // Повертає наступну кістку з набору або std::nullopt (аналог None)
        optional<Domino> operator()() {
        if (dominoes.empty()) {
            return std::nullopt;
        }
        Domino d = dominoes.back();
        dominoes.pop_back();
        return d;
    }

    // Перевірка, чи можна поставити кістку зліва
    bool can_put_left(const deque<Domino>& chain, const Domino& domino) const {
        int left = chain.front().left;
        return (domino.left == left || domino.right == left);
    }

    // Перевірка, чи можна поставити кістку справа
    bool can_put_right(const deque<Domino>& chain, const Domino& domino) const {
        int right = chain.back().right;
        return (domino.left == right || domino.right == right);
    }

    // Додавання кістки зліва
    void put_left(deque<Domino>& chain, Domino domino) const {
        int left = chain.front().left;

        if (domino.right == left) {
                swap(domino.left, domino.right);
        }

        chain.push_front(domino);
    }

    // Додавання кістки справа
    void put_right(deque<Domino>& chain, Domino domino) const {
        int right = chain.back().right;

        if (domino.left == right) {
            chain.push_back(domino);
        }
        else if (domino.right == right) {
               swap(domino.left, domino.right);
            chain.push_back(domino);
        }
    }

    // Створення однієї партії
        deque<Domino> make_deal() {
        dominoes.clear();
        create_dominoes();
        shuffle();

        auto first = (*this)();
        if (!first.has_value()) return {};

            deque<Domino> chain = { first.value() };

        while (true) {
            auto next_domino = (*this)();

            if (!next_domino.has_value()) {
                break;
            }

            Domino domino = next_domino.value();

            bool can_left = can_put_left(chain, domino);
            bool can_right = can_put_right(chain, domino);

            if (!can_left && !can_right) {
                break;
            }

            if (can_left && !can_right) {
                put_left(chain, domino);
                continue;
            }

            if (can_right && !can_left) {
                put_right(chain, domino);
                continue;
            }

            int left = chain.front().left;
            int right = chain.back().right;

            if (left <= right) {
                put_left(chain, domino);
            }
            else {
                put_right(chain, domino);
            }
        }

        return chain;
    }

    // Візуальний вивід усієї партії
    void show_chain(const deque<Domino>& chain, int per_line = 5) const {
        cout << "\n";
        int size = static_cast<int>(chain.size());

        for (int start = 0; start < size; start += per_line) {
            int end = min(start + per_line, size);

            vector<vector<string>> domino_lines;
            for (int i = start; i < end; ++i) {
                domino_lines.push_back(chain[i].get_lines());
            }

            for (int line_number = 0; line_number < 9; ++line_number) {
                for (const auto& domino : domino_lines) {
                    cout << domino[line_number] << "  ";
                }
                cout << "\n";
            }
            cout << "\n";
        }
    }
};


// =========================================================
// КЛАС СТАТИСТИКИ
// =========================================================

class Statistics {
private:
    vector<int> sizes;

public:
    // Додавання результату партії
    void add(int size) {
        sizes.push_back(size);
    }

    // Кількість проведених партій
    int count() const {
        return static_cast<int>(sizes.size());
    }

    // Обчислення середнього значення
    double average() const {
        if (sizes.empty()) return 0.0;
        double sum = accumulate(sizes.begin(), sizes.end(), 0.0);
        return sum / sizes.size();
    }

    // Обчислення медіани
    double median() const {
        if (sizes.empty()) return 0.0;

                vector<int> values = sizes;
            sort(values.begin(), values.end());
        size_t n = values.size();

        if (n % 2 == 1) {
            return values[n / 2];
        }

        return (values[n / 2 - 1] + values[n / 2]) / 2.0;
    }

    // Підрахунок частоти кожного розміру
        map<int, int> frequency() const {
            map<int, int> result;
        for (int size : sizes) {
            result[size]++;
        }
        return result;
    }

    // Пошук найбільш поширеного розміру
    int most_common() const {
        auto freq = frequency();
        if (freq.empty()) return 0;

        int best_size = 0;
        int max_count = -1;

        for (const auto& [size, cnt] : freq) {
            if (cnt > max_count) {
                max_count = cnt;
                best_size = size;
            }
        }
        return best_size;
    }

    // Виведення статистики
    void show() const {
        auto freq = frequency();
        int total = count();

        cout << "\n";
        cout << string(60, '=') << "\n";
        cout << "СТАТИСТИКА\n";
        cout << string(60, '=') << "\n";
        cout << "Кiлькiсть партiй: " << total << "\n\n";

        cout << "Розмiр | Кiлькiсть | Вiдсоток\n";
        cout << string(35, '-') << "\n";

        for (const auto& [size, cnt] : freq) {
            double percent = (total > 0) ? (static_cast<double>(cnt) / total * 100.0) : 0.0;
            cout << setw(6) << size << " | "
                << setw(10) << cnt << " | "
                << setw(7) << fixed << setprecision(2) << percent << "%\n";
        }

        cout << "\n" << string(60, '-') << "\n";
        cout << "Найбiльш поширений розмiр: " << most_common() << "\n";
        cout << "Середнiй розмiр:           " << fixed << setprecision(2) << average() << "\n";
        cout << "Медiанний розмiр:          " << median() << "\n";
    }
};


// =========================================================
// ТЕСТУВАННЯ
// =========================================================

void run_test(DominoDealer& dealer, Statistics& statistics, bool show_all) {
    cout << "\n";
    cout << string(60, '=') << "\n";
    cout << "ТЕСТУВАННЯ ПАРТIЙ\n";
    cout << string(60, '=') << "\n";

    cout << "Кiлькiсть партiй для тесту: ";
    int n = 0;
    cin >> n;
    cout << "\n";

    for (int number = 1; number <= n; ++number) {
        auto chain = dealer.make_deal();

        statistics.add(static_cast<int>(chain.size()));

        if (show_all) {
            cout << "ПАРТIЯ #" << number << "\n";
            cout << "Кiлькiсть кiсток: " << chain.size() << "\n";
            dealer.show_chain(chain);
        }
    }
}


// =========================================================
// ГОЛОВНА ПРОГРАМА
// =========================================================

int main() {
    setlocale(LC_ALL, "uk_UA");

    cout << string(60, '=') << "\n";
    cout << "МОДЕЛЮВАННЯ ГРИ В ДОМИНО\n";
    cout << string(60, '=') << "\n\n";

    cout << "Правила:\n";
    cout << "1. Створюється повний набiр iз 28 кiсток.\n";
    cout << "2. Кiстки випадково перемiшуються.\n";
    cout << "3. Перша кiстка починає партiю.\n";
    cout << "4. Наступнi кiстки можна ставити\n";
    cout << "   злiва або справа.\n";
    cout << "5. Числа на дотичних кiнцях повиннi збiгатися.\n";
    cout << "6. Якщо кiстку не можна поставити,\n";
    cout << "   партiя закiнчується.\n\n";

    DominoDealer dealer;
    Statistics statistics;

    cout << "Виводити кожну партiю повнiстю? (y/n): ";
    string answer;
    cin >> answer;

    bool show_all = (answer == "y" || answer == "Y");

    run_test(dealer, statistics, show_all);

    statistics.show();

    return 0;
}