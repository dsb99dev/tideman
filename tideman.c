#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Max number of candidates
#define MAX 9
#define MAX_NAME_LEN 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
char candidates[MAX][MAX_NAME_LEN + 1];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, char* name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, char* argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
	strcpy(candidates[i], argv[i + 1]);
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    printf("Number of voters: ");
    int voter_count;
    scanf("%i", &voter_count);

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            printf("Rank %i: ", j + 1);
	    char name[20];
	    scanf("%s", name);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

	printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, char* name, int ranks[])
{
	for (int i = 0; i < candidate_count; i++)
	{
		if (strcmp(candidates[i], name) == 0)
		{
			ranks[rank] = i;
			return true;
		}
	}
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
	for (int i = 0; i < candidate_count; i++)
	{
		for (int j = 0; j < candidate_count; j++)
		{
			// find index of i and j in ranks[]
			int k = 0, index_i = -1, index_j = -1;
			while (k < candidate_count)
			{
				if (index_i >= 0 && index_j >=0)
				{
					break;
				}
				if (ranks[k] == i)
				{
					index_i = k;
				}
				if (ranks[k] == j)
				{
					index_j = k;
				}
				k++;
			}
			if (index_i < index_j)
			{
				preferences[i][j]++;
			}
		}
	}
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
	for (int i = 0; i < candidate_count; i++)
	{
		for (int j = 0; j < candidate_count; j++)
		{
			if (preferences[i][j] > preferences[j][i])
			{
				pairs[pair_count].winner = i;
				pairs[pair_count].loser = j;
				pair_count++;
			}
		}
	}
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
	bool sorted;
	do
	{
		sorted = true;
		for (int k = 0; k < pair_count - 1; k++)
		{
			int i_a, i_b, j_a, j_b, strength_a, strength_b;
			// calculate strength of k-th pair
			i_a = pairs[k].winner;
			j_a = pairs[k].loser;
			strength_a = preferences[i_a][j_a] - preferences[j_a][i_a];
			// calculate strength of k+1-th pair
			i_b = pairs[k+1].winner;
			j_b = pairs[k+1].loser;
			strength_b = preferences[i_b][j_b] - preferences[j_b][i_b];
			// sort pairs in decreasing order
			if (strength_a < strength_b)
			{
				// swap pairs
				pair aux;
				aux.winner = pairs[k].winner;
				aux.loser = pairs[k].loser;
				pairs[k].winner = pairs[k+1].winner;
				pairs[k].loser = pairs[k+1].loser;
				pairs[k+1].winner = aux.winner;
				pairs[k+1].loser = aux.loser;
				sorted = false;
			}
		}
	}
	while (!sorted);

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
	for (int i = 0; i < pair_count; i++)
	{
		// check if locking pair(x,y) will not create cycle
		int sum = 0;
		for (int col = 0; col < pair_count; col++)
		{
			if (col != pairs[i].loser)
			{
				for (int row = 0; row < pair_count; row++)
				{
					if (locked[row][col])
					{
						sum++;
						break;
					}
				}
			}
		}
		if (sum + 1 < pair_count)
		{
			locked[pairs[i].winner][pairs[i].loser] = true;
		}
	}
    return;
}

// Print the winner of the election
void print_winner(void)
{
	for (int col = 0; col < pair_count; col++)
	{
		bool winner = true;
		for (int row = 0; row < pair_count; row++)
		{
			if (locked[row][col])
			{
				winner = false;
				break;
			}
		}
		if (winner)
		{
			printf("Winner: %s\n", candidates[col]);
			break;
		}
	}
    return;
}
