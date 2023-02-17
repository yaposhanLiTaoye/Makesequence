#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#define length 1000000
#define samplesize 1000
#define Len (length*samplesize)

char s[Len];

class State {
public:
    State() {}
    ~State() {}
    void Init()
    {
        N = 0;
        m = -1;
        L = 0;
        memset(B, sizeof(B), 0);
        memset(C, sizeof(C), 0);
        B[0] = C[0] = 1;
    }
    void OneRound()
    {
        char d = q[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & q[N - i];
        if (d) {
            if (N + 1 - L > L) {
                char Temp[501];
                for (int i = 1; i <= L; i++)Temp[i] = C[i];
                for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
                for (int i = 1; i <= L; i++)B[i] = Temp[i];
                L = N + 1 - L;
                m = N;
            } else {
                for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
            }
        }
        N++;
    }
    /*pull down the point that is above the line y=0.5x*/
    void PullDownOneRound()
    {
        char d = q[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & q[N - i];

        q[N] ^= d;
        
        N++;
    }
    /*push up the point that is below the line y=0.5x*/
    void PushUpOneRound()
    {
        char d = q[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & q[N - i];

        q[N] ^= ~d;

        char Temp[501];
        for (int i = 1; i <= L; i++)Temp[i] = C[i];
        for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
        for (int i = 1; i <= L; i++)B[i] = Temp[i];
        L = N + 1 - L;
        m = N;

        N++;
    }
    /*whether the point is on the line y=0.5x*/
    bool IsInSlash()
    {
        return 2 * L == N;
    }

    void ShowCurrentLCP()
    {
        Init();
        for (int i = 0; i < 500; i++) {
            OneRound();
            printf("(%3d, %3d)%c", N, L, (i + 1) % 10 == 0 ? '\n' : ' ');
        }
    }
public:
    int N;
    int m;
    int L;
    char B[501];
    char C[501];
    char q[500];
};


/*
 * [n1] is the number of PCT T1s, [n2] is the number of PCT T2s, 
 * mm is the horizontal length of the big PCT,  [gap] is the tail 
 * length.
 *
 */
void MakeSequence(int n1, int n2, int mm, int gap) {
    State curState;
    State backState;
    unsigned char readbuffer[63];
    int frontlen = 500 - n1 * 2 - n2 * 4 - 2 * mm - gap;
    printf("=========================================================================\n");
    printf("n1 = %d\n", n1);
    printf("n2 = %d\n", n2);
    printf("mm = %d\n", mm);
    printf("frontlen = %d\n", frontlen);
    printf("=========================================================================\n");
    // FILE *fillfp = fopen("rdseed.data", "rb");
    for (int wai = 0; wai < 1/*Len / 500*/; wai++) {
        /* fread(readbuffer, sizeof(char), 63, fillfp);
        for (int i = 0; i < 63; i++) {
            for (int j = 0; j < 8; j++) {
                if (8 * i + j < 500) {
                    curState.q[8 * i + j] = readbuffer[i] & 0x80 ? 1 : 0;
                    readbuffer[i] <<= 1;
                }
            }
        }*/
        for (int i = 0; i < 500; i++)
            curState.q[i] = rand() & 1;
        
        /*initialize the Berlekamp-Massey algorithm*/
        curState.Init();
        backState = curState;
        for (int i = 0; i < frontlen; i++) {
            curState.OneRound();
            if (curState.IsInSlash()) {
                backState = curState;
            }
        }
        
        curState = backState;

        int tmpLen = (frontlen - curState.N) / 2;
        if (tmpLen > 0) {
            for (int i = 0; i < tmpLen - 1; i++) {
                curState.OneRound();
            }
            curState.PushUpOneRound();
            for (int i = 0; i < tmpLen; i++) {
                curState.OneRound();
            }
        }

        for (int i = 0; i < n2; i++) {
            curState.PullDownOneRound();
            curState.PushUpOneRound();
            curState.OneRound();
            curState.OneRound();
        }
        
        for (int i = 0; i < n1; i++) {
            curState.PushUpOneRound();
            curState.OneRound();
        }
        
        for (int i = 0; i < mm - 1; i++) {
            curState.PullDownOneRound();
        }
        curState.PushUpOneRound();

        curState.ShowCurrentLCP();
        for (int i = 0; i < 500; i++) {
            s[wai * 500 + i] = curState.q[i];
        }
    }
    //fclose(fillfp);
}

int main(int argc, char **argv) {
    MakeSequence(111, 46, 22, 10);

    /*
    char basename[20] = "biggap10_4_1";
    FILE *fp = fopen(basename, "wb");
    unsigned char buffer[Len / 8];
    for (int i = 0; i < Len / 8; i++) {
        buffer[i] = 0;
        for (int j = 0; j < 8; j++) {
            buffer[i] <<= 1;
            buffer[i] ^= s[i * 8 + j];
        }
    }
    fwrite(buffer, sizeof(unsigned char), Len / 8, fp);
    fclose(fp);
    */
    return 0;
}
