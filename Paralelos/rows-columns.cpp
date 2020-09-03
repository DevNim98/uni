int main(int argc, char const *argv[])
{
    const int MAX = 100;
    double A[MAX][MAX], x[MAX], y[MAX];

    // Initialize A and x, assign y = 0
    for (int i = 0; i < MAX; i++)
    {
        x[i] = i;
        y[i] = 0;
        for (int j = 0; j < MAX; j++)
        {
            A[i][j] = j;
        }        
    }

    // First pair of loops
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            y[i] += A[i][j] * x[j];
        }        
    }

    return 0;
}
