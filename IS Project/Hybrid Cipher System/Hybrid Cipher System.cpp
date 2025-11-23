#include <iostream>                // for input/output
#include <vector>                  // for using vector container
#include <algorithm>              // for reverse, sort, etc.
#include <map>
#include <string>                 // for string operations
#include <cctype>                 // for character checks like isalpha
#include <iomanip>                // for formatting output like setw
#include <numeric>                // for iota (filling a sequence)

using namespace std;

// Caesar Cipher Encryption
string caesarEncrypt(const string& text, int shift) {
    string out = text;
    for (char &c : out) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';  // maintain case
            c = char((c - base + shift + 26) % 26 + base);  // apply shift
        }
    }
    return out;
}

// Caesar Cipher Decryption (inverse of encryption)
string caesarDecrypt(const string& text, int shift) {
    return caesarEncrypt(text, -shift);  // just call encrypt with -shift
}

// Block Reversal (for both encryption/decryption)
string blockReverse(const string& text, int blockSize) {
    string buf;                 // to store characters without spaces
    vector<int> spacePos;      // to remember positions of spaces
    for (int i = 0; i < (int)text.size(); ++i) {
        if (text[i] == ' ') spacePos.push_back(i);  // store space indices
        else buf.push_back(text[i]);               // push non-space chars
    }
    for (int i = 0; i < (int)buf.size(); i += blockSize) {
        int end = min((int)buf.size(), i + blockSize);  // calculate block end
        reverse(buf.begin() + i, buf.begin() + end);     // reverse each block
    }
    string result;
    int bi = 0;                      // index in block reversed buffer
    for (int i = 0; i < (int)text.size(); ++i) {
        if (text[i] == ' ') result.push_back(' ');  // keep original spaces
        else result.push_back(buf[bi++]);           // fill from buffer
    }
    return result;
}

// Create a 6x6 Polybius Square for substitution cipher
vector<vector<char>> createPolybiusSquare(const string& subKey) {
    string content = "abcdefghijklmnopqrstuvwxyz0123456789";  // 36 characters
    vector<vector<char>> square(6, vector<char>(6));  // 6x6 square
    int idx = 0;
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            square[i][j] = content[idx++];  // fill square
    return square;
}

// Display the substitution matrix
void printPolybiusSquare(const vector<vector<char>>& square, const string& label) {
    cout << "\nSubstitution Table:\n    ";
    for (char c : label) cout << c << " ";
    cout << "\n  +--------------------\n";
    for (int i = 0; i < 6; ++i) {
        cout << label[i] << " | ";
        for (int j = 0; j < 6; ++j)
            cout << square[i][j] << " ";
        cout << endl;
    }
}

// Substitution Encryption: convert characters to label pairs
string substitutionEncrypt(const string& text, const vector<vector<char>>& square, const string& label) {
    string result;
    for (char c : text) {
        for (int i = 0; i < 6; ++i)
            for (int j = 0; j < 6; ++j)
                if (square[i][j] == c)  // match character
                    result += label[i], result += label[j];  // append label pair
    }
    return result;
}

// Substitution Decryption: convert label pairs back to characters
string substitutionDecrypt(const string& text, const vector<vector<char>>& square, const string& label) {
    string result;
    for (size_t i = 0; i < text.length(); i += 2) {
        int row = label.find(text[i]);         // find row label
        int col = label.find(text[i + 1]);     // find col label
        if (row != string::npos && col != string::npos) {
            result += square[row][col];        // get original char
        }
    }
    return result;
}

// Print Transposition Table (grid)
void printTranspositionTable(const string& keyword, const string& intermediate) {
    int cols = keyword.size();                            // number of columns
    int rows = (intermediate.size() + cols - 1) / cols;   // calculate rows
    vector<vector<char>> grid(rows, vector<char>(cols, 'X'));  // fill with X
    for (int i = 0; i < (int)intermediate.size(); ++i)
        grid[i / cols][i % cols] = intermediate[i];  // fill grid row-wise

    cout << "\nTransposition Table:\n";
    for (char c : keyword) cout << setw(3) << c;
    cout << "\n";
    for (int i = 0; i < cols; ++i) cout << "---";
    cout << endl;
    for (const auto& row : grid) {
        for (char c : row)
            cout << setw(3) << c;
        cout << endl;
    }
}

// Transposition Encryption (columnar based on sorted key)
string transpositionEncrypt(const string& keyword, const string& intermediate) {
    int cols = keyword.size();
    int rows = (intermediate.size() + cols - 1) / cols;
    vector<vector<char>> grid(rows, vector<char>(cols, 'X'));
    for (int i = 0; i < (int)intermediate.size(); ++i)
        grid[i / cols][i % cols] = intermediate[i];

    vector<int> order(cols);
    iota(order.begin(), order.end(), 0);  // fill 0 to n-1
    sort(order.begin(), order.end(), [&](int a, int b) {
        return keyword[a] < keyword[b];   // sort by keyword
    });

    string cipher;
    for (int idx : order)
        for (int i = 0; i < rows; ++i)
            cipher += grid[i][idx];      // read columns by sorted order

    return cipher;
}

// Transposition Decryption
string transpositionDecrypt(const string& keyword, const string& cipher) {
    int cols = keyword.size();
    int rows = (cipher.size() + cols - 1) / cols;
    vector<vector<char>> grid(rows, vector<char>(cols, 'X'));

    vector<int> order(cols);
    iota(order.begin(), order.end(), 0);
    sort(order.begin(), order.end(), [&](int a, int b) {
        return keyword[a] < keyword[b];  // same sorting as encryption
    });

    int k = 0;
    for (int idx : order)
        for (int i = 0; i < rows; ++i)
            if (k < cipher.size())
                grid[i][idx] = cipher[k++];  // fill columns by sorted order

    printTranspositionTable(keyword, cipher);  // show the table

    string intermediate;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            intermediate += grid[i][j];   // read row-wise

    return intermediate;
}

int main() {
    while (true) {  // loop until user exits
        string mode;
        cout << "\nDo you want to (encrypt/decrypt)? ";
        cin >> mode;
        cin.ignore();  // discard newline

        if (mode == "e") {  // encryption path
            string text;
            cout << "Enter plaintext (A-Z, 0-9): ";
            getline(cin, text);

            int shift;
            cout << "Enter Caesar shift: ";
            cin >> shift;
            cin.ignore();

            string caesar = caesarEncrypt(text, shift);
            cout << "\nCaesar Cipher Text: " << caesar << endl;

            int blockSize;
            while (true) {
                cout << "Enter block size for reversal: ";
                cin >> blockSize;
                if (cin.fail()) {
                    cin.clear(); cin.ignore(1000, '\n');
                    cout << "\nInvalid input. Enter numeric value.\n";
                } else break;
            }
            cin.ignore();

            string reversed = blockReverse(caesar, blockSize);
            cout << "\nBlock Reversal Text: " << reversed << endl;

            string subKey;
            do {
                cout << "Enter 6-letter substitution key (e.g. ADFGVX): ";
                getline(cin, subKey);
                if (subKey.size() != 6) cout << "\nInvalid key. Please enter exactly 6 characters.\n";
            } while (subKey.size() != 6);

            auto square = createPolybiusSquare(subKey);
            printPolybiusSquare(square, subKey);

            string substituted = substitutionEncrypt(reversed, square, subKey);
            cout << "\nSubstituted Text: " << substituted << endl;

            string keyword;
            do {
                cout << "Enter transposition key: ";
                getline(cin, keyword);
                if (keyword.size() < 2) cout << "\nInvalid key. Please enter a longer keyword.\n";
            } while (keyword.size() < 2);

            printTranspositionTable(keyword, substituted);
            string cipher = transpositionEncrypt(keyword, substituted);
            cout << "\nFinal Encrypted Ciphertext: " << cipher << endl;

        } else if (mode == "d") {  // decryption path
            string cipher;
            cout << "Enter ciphertext: ";
            getline(cin, cipher);

            int shift;
            cout << "Enter Caesar shift: ";
            cin >> shift;
            cin.ignore();

            int blockSize;
            while (true) {
                cout << "Enter block size for reversal: ";
                cin >> blockSize;
                if (cin.fail()) {
                    cin.clear(); cin.ignore(1000, '\n');
                    cout << "\nInvalid input. Enter numeric value.\n";
                } else break;
            }
            cin.ignore();

            string subKey;
            do {
                cout << "Enter 6-letter substitution key (e.g. ADFGVX): ";
                getline(cin, subKey);
                if (subKey.size() != 6) cout << "\nInvalid key. Please enter exactly 6 characters.\n";
            } while (subKey.size() != 6);

            auto square = createPolybiusSquare(subKey);
            printPolybiusSquare(square, subKey);

            string keyword;
            do {
                cout << "Enter transposition key: ";
                getline(cin, keyword);
                if (keyword.size() < 2) cout << "\nInvalid key. Please enter a longer keyword.\n";
            } while (keyword.size() < 2);

            string decryptedTrans = transpositionDecrypt(keyword, cipher);
            cout << "\nDecrypted Transposition: " << decryptedTrans;

            string decryptedSub = substitutionDecrypt(decryptedTrans, square, subKey);
            cout << "\nDecrypted Substitution: " << decryptedSub;

            string decryptedBlock = blockReverse(decryptedSub, blockSize);
            cout << "\nDecrypted Block Reversal: " << decryptedBlock;

            string decryptedText = caesarDecrypt(decryptedBlock, shift);
            cout << "\nFinal Decrypted Text: " << decryptedText << endl;

        } else {
            cout << "\nInvalid option. Please type 'encrypt' or 'decrypt'.\n";
            continue;
        }

        string again;
        cout << "\nDo you want to continue (y/n)? ";
        getline(cin, again);
        if (again != "y" && again != "yes") break;  // exit loop if user says no
    }
    return 0;
}
