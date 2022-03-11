#pragma once

double eval(string expr) {
    try {
        string spacelessExpr; // Get Rid of Spaces
        for (int i = 0; i < expr.length(); i++) {
            if (expr[i] != ' ') {
                spacelessExpr += expr[i];
            }
        }

        string tok = ""; // Do parantheses first
        for (int i = 0; i < spacelessExpr.length(); i++) {
            if (spacelessExpr[i] == '(') {
                int iter = 1;
                string token;
                i++;
                while (true) {
                    if (spacelessExpr[i] == '(') {
                        iter++;
                    } else if (spacelessExpr[i] == ')') {
                        iter--;
                        if (iter == 0) {
                            i++;
                            break;
                        }
                    }
                    token += spacelessExpr[i];
                    i++;
                }
                tok += to_string(eval(token));
            }
            tok += spacelessExpr[i];
        }

        for (int i = 0; i < tok.length(); i++) {
            if (tok[i] == '>') {
                return eval(tok.substr(0, i)) > eval(tok.substr(i + 1, tok.length() - i - 1));
            } else if (tok[i] == '<') {
                return eval(tok.substr(0, i)) < eval(tok.substr(i + 1, tok.length() - i - 1));
            } else if (tok.substr(i, 2) == "==") {
                return eval(tok.substr(0, i)) == eval(tok.substr(i + 2, tok.length() - i - 2));
            }
        }

        for (int i = 0; i < tok.length(); i++) {
            if (tok[i] == '+') {
                return eval(tok.substr(0, i)) + eval(tok.substr(i + 1, tok.length() - i - 1));
            } else if (tok[i] == '-') {
                return eval(tok.substr(0, i)) - eval(tok.substr(i + 1, tok.length() - i - 1));
            }
        }

        for (int i = 0; i < tok.length(); i++) {
            if (tok[i] == '*') {
                return eval(tok.substr(0, i)) * eval(tok.substr(i + 1, tok.length() - i - 1));
            } else if (tok[i] == '/') {
                return eval(tok.substr(0, i)) / eval(tok.substr(i + 1, tok.length() - i - 1));
            }
        }

        for (int i = 0; i < tok.length(); i++) {
            if (tok.substr(i, 2) == "&&") {
                return eval(tok.substr(0, i)) && eval(tok.substr(i + 2, tok.length() - i - 2));
            } else if (tok.substr(i, 2) == "||") {
                return eval(tok.substr(0, i)) || eval(tok.substr(i + 2, tok.length() - i - 2));
            }
        }

        if (tok.substr(0, 1) == "!") {
            return !eval(tok.substr(1).c_str());
        }

        if (tok.substr(0, 4) == "exec") {
            return system(tok.substr(5, tok.length() - 6).c_str());
        }

        if (tok.substr(0, 2) == "${") {
            return 0;
        }

        return stod(tok.c_str()); // Return the value...
    } catch (...) {
        fprintf(stderr, "Unable to evaluate expression %s!\n", expr.c_str());
    }
}