#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i, 2d array size is set to the constants
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1]; // stores whatever names are typed in command line
        candidates[i].votes = 0; // sets initial votes to 0
        candidates[i].eliminated = false; // initially set to not elimnated as vote hasn't been started
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes (i)
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank (j)
        for (int j = 0; j < candidate_count; j++)
        {
            // Asking for you to enter candidate name for Rank 1, Rank 2, Rank 3, etc. up to candidate count
            string name = get_string("Rank %i: ", j + 1);

            // calls vote() and records vote. If the vote() returns false (i.e. ith voter, jth rank, name) doesn't match; the vote is invalid
            // !vote() is saying vote () line 148 returned false
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            // Ends this while loop, which ends the program
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
// Passed i, j, name entered (line 78)
bool vote(int voter, int rank, string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
       if (strcmp(candidates[i].name, name) == 0)
       {
           // Updating the ith voters jth preference. If the name is present,
           // add the number (of the iteration) i as a rank in the pref array
           // i.e. voter0 votes for Bob as rank 1. Bob is at index1 in the candidates array, so the vote() fuction will
           // add to the array preferences[0][0] = 1 then return true, which makes the main() function ask the voter for
           // their second preference. Alice is at index0 in candidates array, so preferences[0][1] = 0, etc.
           preferences[voter][rank] = i;
           return true;
       }
    }
    return false; // Candidate name not found
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    // Iterate through the voters (i)
    for (int i = 0; i < voter_count; i++)
    {
        // Iterate through the voters rank preference, j (up to the number of candidates)
        for (int j = 0; j < candidate_count; j++)
        {
            // Check preferences array for elimination, if true it will go to the next rank (j) position of that voter
            if (candidates[preferences[i][j]].eliminated == false)
            {
                // Updating how many votes each candidate recieved, and in which rank
                candidates[preferences[i][j]].votes++;
                // We have updated the votes for that candidate and we break out of the for (int j) loop to the next voter (i)
                break;
            }
        }
    }
    return;
}

// Print the winner of the election, if there is one. Nothing is passed into this from the caller
bool print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        string majority_votes = candidates[i].name;
        // Must have more than half the votes in order to win
        if (candidates[i].votes > voter_count / 2)
        {
            printf("%s\n", majority_votes);
            return true;
        }
    }
    return false; // No winners yet
}

// Return the minimum number of votes any remaining candidate has (no inputs passed); looking for least number of votes
int find_min(void)
{
    int min_votes = voter_count;
    // Iterate through the each candidate, checking their number of votes
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes < min_votes && candidates[i].eliminated == false)
        {
            // This updates our variable (line 192) so we always loop through the candidates, comparing their number of
            // votes to the minimum we have seen so far (a linear search)
            min_votes = candidates[i].votes;
        }
    }
    return min_votes;
}

// Return true if the election is tied between all candidates, false otherwise
// the input (min) is actually the same as min_votes from the find_min() function, that was passed back into main (line 103)
bool is_tie(int min)
{
    // Iterate through the candidates, if their votes are not = to min, then there is no tie.
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false && candidates[i].votes != min)
        {
            return false;
        }
    }
    // True means there is a tie
    return true;
}

// Eliminate the candidate (or candidates) in last place
// min is still the min_votes from find_min() but has been reassigned in main(). The tie condition has been checked, so
// once this has been called we can eliminate the candidate with the least votes
void eliminate(int min)
{
    // iterate though candidates. If they have the least amount of votes (min), eliminate them
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == min)
        {
            candidates[i].eliminated = true;
        }
    }
    return;
}
