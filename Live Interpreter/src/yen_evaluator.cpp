#include "yen_evaluator.h"
#include <cstdlib>

enum class states
{
    q0, F, S1, S2, Op, M, Z
};

bool float_p(char c) {
    return c == '0' ||
        c == '1' ||
        c == '2' ||
        c == '3' ||
        c == '4' ||
        c == '5' ||
        c == '6' ||
        c == '7' ||
        c == '8' ||
        c == '9' ||
        c == '.';
}

std::string yen_evaluator(const char* buf, const size_t buf_size)
{
	auto current_state = states::q0;
    char last_op = '\0';
    char num[256] = "";
    float answer = 1.f;
    float n;
    size_t i = 0;

    for (size_t j = 0; j < buf_size; j++)
    {
	    const char c = buf[j];

        if (c == '\0') {
            if (current_state == states::F) {
                n = atof(num);
                num[0] = '\0';
                i = 0;

                if (last_op == '\0' || last_op == '*')
                    answer *= n;
                else if (last_op == '/') {
                    if (n == 0)
                        current_state = states::Z;
                    answer /= n;
                }
            }

            break;
        }

        switch (current_state)
        {
        case states::q0:
            if (c == ' ' || c == '\n') {

            }
            else if (float_p(c)) {
                num[i] = c;
                i++;
                num[i] = '\0';
                current_state = states::F;
            }
            else
                current_state = states::M;
            break;
        case states::F:
            if (c == ' ' || c == '\n') {
                current_state = states::S1;
                n = atof(num);
                num[0] = '\0';
                i = 0;

                if (last_op == '\0' || last_op == '*')
                    answer *= n;
                else if (last_op == '/') {
                    if (n == 0.f)
                        current_state = states::Z;
                    answer /= n;
                }
            }
            else if (c == '*' || c == '/') {
                current_state = states::Op;
                n = atof(num);
                num[0] = '\0';
                i = 0;

                if (last_op == '\0' || last_op == '*')
                    answer *= n;
                else if (last_op == '/') {
                    if (n == 0)
                        current_state = states::Z;
                    answer /= n;
                }

                last_op = c;
            }
            else if (float_p(c)) {
                num[i] = c;
                i++;
                num[i] = '\0';
            }
            else
                current_state = states::M;
            break;
        case states::S1:
            if (c == ' ' || c == '\n') {}
            else if (c == '*' || c == '/') {
                current_state = states::Op;
                last_op = c;
            }
            else
                current_state = states::M;
            break;
        case states::S2:
            if (c == ' ' || c == '\n') {}
            else if (float_p(c)) {
                num[i] = c;
                i++;
                num[i] = '\0';
                current_state = states::F;
            }
            else
                current_state = states::M;
            break;
        case states::Op:
            if (c == ' ' || c == '\n') {
                current_state = states::S2;
            }
            else if (float_p(c)) {
                num[i] = c;
                i++;
                num[i] = '\0';
                current_state = states::F;
            }
            else
                current_state = states::M;
            break;
        case states::M:
            break;
        default:
            break;
        }
    }

    switch (current_state)
    {
    case states::q0:
    case states::Op:
    case states::S2:
    case states::M:
        return "malformed expression";
    case states::Z:
        return "division by zero";
    default:
        return std::format("{}", answer);
    }
}