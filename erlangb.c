#include <stdio.h>
#include <string.h>
#include <stdlib.h>

float lost_tickets(float a, int v);
float average_number_of_busy_channels(float E, float a);
int required_number_of_channels(float E, float a);
float required_load(float E, int v);
float find_lost_via_m(float a, float m);
float find_a_from_v_m(int v, float m);
float find_a_for_m_e(float m, float E);

int help_message();



int main(int argc, char **argv) {
    if (argc == 1) {
            help_message();
            return 0;
        }
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-h") == 0 && argc == 2) {
            help_message();
            return 0;
        }
        //  1 a v - E m
        if (strcmp(argv[i], "-a") == 0 && strcmp(argv[i+2], "-v") == 0) {
            float a = atof(argv[i+1]);
            int v = atoi(argv[i+3]);

            float E = lost_tickets(a, v);
            float M = average_number_of_busy_channels(E, a);
            
            if (v < 0 || a < 0) {
                printf("Invalid input\n");
                help_message();
                return 1;
            }
            printf("Percentage of lost %f\n", E);
            printf("Average number of busy channels: %f\n", M);

            return 0;
        } 
        // 2 a E - v m
        if (strcmp(argv[i], "-a") == 0 && strcmp(argv[i+2], "-E") == 0) {
            float a = atof(argv[i+1]);
            float E = atof(argv[i+3]);

            if ((E < 0 || E > 1) || a < 0) {
                printf("Invalid input\n");
                help_message();
                return 1;
            }
            int v = required_number_of_channels(E, a);
            float M = average_number_of_busy_channels(E, a);
            printf("Required number of channels: %d\n", v);
            printf("Average number of busy channels: %f\n", M);
            return 0;

        }
        // 5 v E - a m
        if (strcmp(argv[i], "-v") == 0 && strcmp(argv[i+2], "-E") == 0) {
            int v = atoi(argv[i+1]);
            float E = atof(argv[i+3]);
            if ((E < 0 || E > 1) || v < 0) {
                printf("Invalid input\n");
                help_message();
                return 1;
            }
            float a = required_load(E, v);
            float M = average_number_of_busy_channels(E, a);

            printf("Load: %f\n", a);
            printf("Average number of busy channels: %f\n", M);
            return 0;
        }
        // 3 a m - v E
        if (strcmp(argv[i], "-a") == 0 && strcmp(argv[i+2], "-m") == 0) {
            float a = atof(argv[i+1]);
            float m = atof(argv[i+3]);
            if (a < 0 || m < 0) {
                printf("Invalid input\n");
                help_message();
                return 1;
            }
            float E = find_lost_via_m(a,  m);
            int v = required_number_of_channels(E, a);
            printf("Percentage of lost: %f\n", E);
            printf("Required number of channels:%d\n", v);
            return 0;
        }
        // 4 v m - a E
        if (strcmp(argv[i], "-v") == 0 && strcmp(argv[i+2], "-m") == 0) {
            int v = atoi(argv[i+1]);
            float m = atof(argv[i+3]);
            if (v < 0 || m < 0) {
                printf("Invalid input\n");
                help_message();
                return 1;
            }
            float a = find_a_from_v_m(v, m);
            float E = lost_tickets(a, v);
            printf("Percentage of lost: %f\n", E);
            printf("Load: %f\n", a);
            
            
            return 0;
        }
        // 6 E m - a v
        if (strcmp(argv[i], "-E") == 0 && strcmp(argv[i+2], "-m") == 0) {
            float E = atof(argv[i+1]);
            float m = atof(argv[i+3]);
            float a = find_a_for_m_e(m, E);
            int v = required_number_of_channels(E, a);
            printf("%f\n%d\n", a,v);
            
        } 
    }
}                

float find_a_for_m_e(float m, float E) {
    return m / (1.0 - E);
}
float lost_tickets(float a, int v) {
    int k = 1.0;
    float r = 1.0;
    float r_sum = 1.0;
    float e;
    while (k <= v) {
       r = r * ((v - k + 1.0)/a);
       r_sum += r;
       k = k + 1.0;
    }
    e = 1.0 / r_sum;

    return e;
}


float average_number_of_busy_channels(float E, float a) {
    float m = a * (1.0 - E);
    return m;
}
//Бисекции
float find_lost_via_m(float a, float m) {
    float left = 0.0f;
    float right = 1.0f;
    float mid, fmid;
    float eps = 1e-6;

    while ((right - left) > eps) {
        mid = (left + right) / 2.0f;
        fmid = average_number_of_busy_channels(mid, a);

        if (fmid > m) {
            left = mid;
        } else {
            right = mid;
        }
    }

    return (left + right) / 2.0f;
    
}

//Бинарный поиск
int required_number_of_channels(float E, float a) {
    int left = 1;
    int right = 10000;
    int mid;
    float loss;

    while (left < right) {
        mid = (left + right) / 2;
        loss = lost_tickets(a, mid);

        if (loss > E) {
            // Каналов мало, вероятность отказа слишком велика
            left = mid + 1;
        } else {
            // Потери <= желаемого E — пробуем меньше каналов
            right = mid;
        }
    }

    return left;
}


float required_load(float E, int v) {
    float a = 0.00001f;
    while (lost_tickets(a, v) <= E) {
        a = a + 0.01f;
    }

    return a;
}

float find_a_from_v_m(int v, float m) {
    float left = 0.000001f;
    float right = v + 5000;    
    float mid;
    float eps = 1e-6;

    while ((right - left) > eps) {
        mid = (left + right) / 2.0f;

        float E = lost_tickets(mid, v);     
        float m_calc = mid * (1.0f - E);   

        if (m_calc < m){
            left = mid;
        } else {
            right = mid;
        }
    }

    return (left + right) / 2.0f;
}

int help_message() {
    printf("erlangb is Erlang B calculator, it accepts the following arguments and their values as input:\n");
    printf("-h show this message\n");
    printf("-a Load > 0\n");
    printf("-v Number of channels, integer\n");
    printf("-m Average number of busy channels\n");
    printf("-E Percentage of lost tickets 0 < E < 1\n");
    printf("Here is all supported cases of use:\n");
    printf("-a <number> -v <number> -> E, m\n");
    printf("-a <number> -E <number> -> v, m\n");
    printf("-a <number> -m <number> -> v, E\n");
    printf("-v <number> -m <number> -> a, E\n");
    printf("-E <number> -m <number> -> a, v\n");
    printf("Example:\n");
    printf("Input:\n    -a 10 -v 5\n");
    printf("Output:\n");
    printf("    Percentage of lost 0.563952\n");
    printf("    Average number of busy channels: 4.360478\n");
    return 0;

}
