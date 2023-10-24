#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef struct song {
    char title[100];
    char artist[100];
    char genre[50];
    int year;
    struct song *left;
    struct song *right;
    int height;
} Song;

typedef struct artistNode {
    char artist[100];
    Song *song;  // Pointer to a song with the artist
    struct artistNode *next;
} ArtistNode;

typedef struct symbolTable {
    ArtistNode *head;
} SymbolTable;

ArtistNode *createArtistNode(const char *artist, Song *song) {
    ArtistNode *node = (ArtistNode *)malloc(sizeof(ArtistNode));
    strcpy(node->artist, artist);
    node->song = song;
    node->next = NULL;
    return node;
}

// Function to insert an artist-node into the symbol table
void insertArtistNode(SymbolTable *table, const char *artist, Song *song) {
    ArtistNode *node = createArtistNode(artist, song);
    if (table->head == NULL) {
        table->head = node;
    } else {
        node->next = table->head;
        table->head = node;
    }
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(Song *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

int getBalance(Song *node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

Song *rightRotate(Song *y) {
    Song *x = y->left;
    Song *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

Song *leftRotate(Song *x) {
    Song *y = x->right;
    Song *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

Song *insert(Song *node, char title[], char artist[], char genre[], int year) {
    if (node == NULL) {
        node = (Song *)malloc(sizeof(Song));
        strcpy(node->title, title);
        strcpy(node->artist, artist);
        strcpy(node->genre, genre);
        node->year = year;
        node->left = node->right = NULL;
        node->height = 1;
        return node;
    }

    int cmp = strcmp(title, node->title);
    if (cmp < 0) {
        node->left = insert(node->left, title, artist, genre, year);
    } else if (cmp > 0) {
        node->right = insert(node->right, title, artist, genre, year);
    } else {
        printf("Song already exists\n");
        return node; // Don't insert duplicates
    }

    // Update height
    node->height = 1 + max(height(node->left), height(node->right));

    // Get the balance factor
    int balance = getBalance(node);

    // Perform rotations if needed
    if (balance > 1 && strcmp(title, node->left->title) < 0) {
        return rightRotate(node);
    }
    if (balance < -1 && strcmp(title, node->right->title) > 0) {
        return leftRotate(node);
    }
    if (balance > 1 && strcmp(title, node->left->title) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && strcmp(title, node->right->title) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

int stricmp(const char *a, const char *b) {
    while (*a && *b) {
        int diff = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (diff != 0) {
            return diff;
        }
        ++a;
        ++b;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

Song *findSongByTitle(Song *root, char *title) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = stricmp(title, root->title);
    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return findSongByTitle(root->left, title);
    } else {
        return findSongByTitle(root->right, title);
    }
}

Song *insertInResultList(Song *result, Song *song) {
    if (result == NULL) {
        result = song;
        result->right = NULL;
    } else {
        Song *temp = result;
        while (temp->right != NULL) {
            temp = temp->right;
        }
        temp->right = song;
        song->right = NULL;
    }
    return result;
}

Song *findSongsByArtist(Song *root, const char *artist, Song *result) {
    if (root == NULL) {
        return result;
    }

    int cmp = stricmp(artist, root->artist);
    if (cmp == 0) {
        result = insertInResultList(result, root);
    }

    result = findSongsByArtist(root->left, artist, result);
    result = findSongsByArtist(root->right, artist, result);

    return result;
}

Song *findSongsByGenre(Song *root, const char *genre) {
    if (root == NULL) {
        return NULL;
    }

    Song *result = NULL;

    int cmp = stricmp(genre, root->genre);
    if (cmp == 0) {
        // The current song matches the genre; add it to the result list
        result = root;
    }

    // Recursively search for more songs in the left and right subtrees
    Song *leftResult = findSongsByGenre(root->left, genre);
    Song *rightResult = findSongsByGenre(root->right, genre);

    // Concatenate the result list with left and right results
    if (leftResult != NULL) {
        if (result != NULL) {
            // If result already contains a song, append the left result
            Song *last = result;
            while (last->right != NULL) {
                last = last->right;
            }
            last->right = leftResult;
        } else {
            result = leftResult;
        }
    }

    // Similarly, concatenate the result list with the right result
    if (rightResult != NULL) {
        if (result != NULL) {
            // If result already contains a song, append the right result
            Song *last = result;
            while (last->right != NULL) {
                last = last->right;
            }
            last->right = rightResult;
        } else {
            result = rightResult;
        }
    }

    return result;
}

Song *concatenateLists(Song *list1, Song *list2) {
    if (list1 == NULL) {
        return list2;
    }
    if (list2 == NULL) {
        return list1;
    }

    Song *current = list1;
    while (current->right != NULL) {
        current = current->right;
    }

    current->right = list2;
    list2->left = current;

    return list1;
}

// Function to find songs by year
Song* findSongsByYear(Song* root, int year, int* count) {
    if (root == NULL) {
        *count = 0;
        return NULL;
    }

    Song* leftFiltered = findSongsByYear(root->left, year, count);
    Song* rightFiltered = findSongsByYear(root->right, year, count);

    if (root->year == year) {
        if (leftFiltered == NULL) {
            leftFiltered = root;
            leftFiltered->right = NULL;
        } else {
            Song* temp = leftFiltered;
            while (temp->right != NULL) {
                temp = temp->right;
            }
            temp->right = root;
            root->right = NULL;
        }
        (*count)++;
    }

    return concatenateLists(concatenateLists(leftFiltered, rightFiltered), NULL);
}


Song *findMin(Song *node) {
    if (node->left != NULL) {
        return findMin(node->left);
    }
    return node;
}

Song *deleteNode(Song *node, char title[]) {
    if (node == NULL) {
        printf("Song with title '%s' not found in the playlist. Cannot delete.\n", title);
        return node; // Return the same node if the song is not found
    }

    int cmp = stricmp(title, node->title);
    if (cmp < 0) {
        node->left = deleteNode(node->left, title);
    } else if (cmp > 0) {
        node->right = deleteNode(node->right, title);
    } else {
        if (node->left == NULL) {
            Song *temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL) {
            Song *temp = node->left;
            free(node);
            return temp;
        } else {
            Song *temp = findMin(node->right);
            strcpy(node->title, temp->title);
            strcpy(node->artist, temp->artist);
            strcpy(node->genre, temp->genre);
            node->year = temp->year;
            node->right = deleteNode(node->right, temp->title);
        }
    }

    // Update height
    node->height = 1 + max(height(node->left), height(node->right));

    // Get the balance factor
    int balance = getBalance(node);

    // Perform rotations if needed
    if (balance > 1 && getBalance(node->left) >= 0) {
        return rightRotate(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0) {
        return leftRotate(node);
    }
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void inorder(Song *node) {
    if (node != NULL) {
        inorder(node->left);
        printf("%s by %s (%s, %d)\n", node->title, node->artist, node->genre, node->year);
        inorder(node->right);
    } else {
        printf("Playlist is empty.\n");
    }
}

void shufflePlaylist(Song *root, Song *shuffled[], int *index) {
    if (root == NULL) {
        return;
    }

    shufflePlaylist(root->left, shuffled, index);

    // Randomly select an index to insert the current song
    int randomIndex = rand() % (*index + 1);
    if (randomIndex < *index) {
        shuffled[*index] = shuffled[randomIndex]; // Move the existing song to the end
    }
    shuffled[randomIndex] = root; // Insert the current song at the random index
    (*index)++;

    shufflePlaylist(root->right, shuffled, index);
}

void printShuffledPlaylist(Song *shuffled[], int size) {
	int i;
    for ( i = 0; i < size; i++) {
        printf("%s by %s (%s, %d)\n", shuffled[i]->title, shuffled[i]->artist, shuffled[i]->genre, shuffled[i]->year);
    }
}

int getSize(Song *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + getSize(node->left) + getSize(node->right);
}

void findMostCommonArtistHelper(Song *node, char *mostCommonArtist, int *maxCount, char *currentArtist, int *currentCount) {
    if (node == NULL) {
        return;
    }

    findMostCommonArtistHelper(node->left, mostCommonArtist, maxCount, currentArtist, currentCount);

    if (strcmp(node->artist, currentArtist) == 0) {
        (*currentCount)++;
    } else {
        strcpy(currentArtist, node->artist);
        (*currentCount) = 1;
    }

    if (*currentCount > *maxCount) {
        strcpy(mostCommonArtist, currentArtist);
        (*maxCount) = (*currentCount);
    }

    findMostCommonArtistHelper(node->right, mostCommonArtist, maxCount, currentArtist, currentCount);
}

void findMostCommonArtist(Song *root, char *mostCommonArtist, int *maxCount) {
    if (root == NULL) {
        printf("Playlist is empty.\n");
        return;
    }

    char currentArtist[100];
    int currentCount = 0;
    *maxCount = 0;
    findMostCommonArtistHelper(root, mostCommonArtist, maxCount, currentArtist, &currentCount);
}

void findMostCommonGenreHelper(Song *node, char *mostCommonGenre, int *maxCount, char *currentGenre, int *currentCount) {
    if (node == NULL) {
        return;
    }

    findMostCommonGenreHelper(node->left, mostCommonGenre, maxCount, currentGenre, currentCount);

    if (strcmp(node->genre, currentGenre) == 0) {
        (*currentCount)++;
    } else {
        strcpy(currentGenre, node->genre);
        (*currentCount) = 1;
    }

    if (*currentCount > *maxCount) {
        strcpy(mostCommonGenre, currentGenre);
        (*maxCount) = (*currentCount);
    }

    findMostCommonGenreHelper(node->right, mostCommonGenre, maxCount, currentGenre, currentCount);
}

void findMostCommonGenre(Song *root, char *mostCommonGenre, int *maxCount) {
    if (root == NULL) {
        printf("Playlist is empty.\n");
        return;
    }

    char currentGenre[50];
    int currentCount = 0;
    *maxCount = 0;
    findMostCommonGenreHelper(root, mostCommonGenre, maxCount, currentGenre, &currentCount);
}


int main() {
    Song *root = NULL;
    SymbolTable artistTable;
    artistTable.head = NULL;
    int choice;
    char inputBuffer[1024]; // Buffer for reading input

    // Seed the random number generator with the current time
    srand((unsigned int)time(NULL));

    while (1) {
        printf("\nMusic Playlist Organizer\n");
        printf("-----------------------\n");
        printf("1. Add a song\n");
        printf("2. Filter\n"); // Main menu for filtering
        printf("3. Delete a song\n");
        printf("4. Shuffle playlist\n");
        printf("5. Find most common artist\n");
        printf("6. Find most common genre\n");
        printf("7. Print playlist\n");
        printf("8. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character left in the input buffer

        switch (choice) {
                        case 1: {
                // Add a song
                char title[100], artist[100], genre[50];
                int year;

                printf("Enter song title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0'; // Remove the newline character

                // Validate that the title is not empty
                if (strlen(title) == 0) {
                    printf("Title cannot be empty. Please enter a valid title.\n");
                    break;
                }

                // Check if a song with the same title already exists
                if (findSongByTitle(root, title) != NULL) {
                    printf("A song with the same title already exists. Please enter a different title.\n");
                    break;
                }

                printf("Enter artist name: ");
                fgets(artist, sizeof(artist), stdin);
                artist[strcspn(artist, "\n")] = '\0'; // Remove the newline character

                // Validate that the artist is not empty
                if (strlen(artist) == 0) {
                    printf("Artist name cannot be empty. Please enter a valid artist name.\n");
                    break;
                }

                printf("Enter genre: ");
                fgets(genre, sizeof(genre), stdin);
                genre[strcspn(genre, "\n")] = '\0'; // Remove the newline character

                // Validate that the genre is not empty
                if (strlen(genre) == 0) {
                    printf("Genre cannot be empty. Please enter a valid genre.\n");
                    break;
                }

                printf("Enter year: ");
                fgets(inputBuffer, sizeof(inputBuffer), stdin);
                year = atoi(inputBuffer); // Convert the input to an integer

                // Validate that the year is greater than zero
                if (year <= 0) {
                    printf("Year should be a positive number. Please enter a valid year.\n");
                    break;
                }

                // Check if a song with the same details already exists
                if (findSongByTitle(root, title) != NULL) {
                    printf("A song with the same details already exists. Please enter a different song.\n");
                    break;
                }

                // Insert the song into the AVL tree
                root = insert(root, title, artist, genre, year);

                // Update the symbol table with the new song
                insertArtistNode(&artistTable, artist, root);

                printf("Song added successfully\n");
                break;
            }


            case 2: {
                // Filter submenu
                int filterChoice;

                printf("\nFilter by:\n");
                printf("1. Title\n");
                printf("2. Artist\n");
                printf("3. Genre\n");
                printf("4. Year\n");
                printf("5. Back to main menu\n");

                printf("\nEnter your choice: ");
                scanf("%d", &filterChoice);
                getchar(); // Consume the newline character left in the input buffer

                switch (filterChoice) {
                    case 1: {
                        // Filter by title
                        char title[100];

                        printf("Enter song title to filter: ");
                        fgets(title, sizeof(title), stdin);
                        title[strcspn(title, "\n")] = '\0'; // Remove the newline character

                        Song *filtered = findSongByTitle(root, title);

                        if (filtered == NULL) {
                            printf("Song not found\n");
                        } else {
                            printf("%s by %s (%s, %d)\n", filtered->title, filtered->artist, filtered->genre, filtered->year);
                        }
                        break;
                    }

                    case 2: {
                        // Filter by artist
                        char artist[100];

                        printf("Enter artist name to filter: ");
                        fgets(artist, sizeof(artist), stdin);
                        artist[strcspn(artist, "\n")] = '\0'; // Remove the newline character

                        // Initialize the result list to NULL
                        Song *filtered = NULL;

                        // Search for songs by artist and store them in the result list
                        filtered = findSongsByArtist(root, artist, filtered);

                        if (filtered == NULL) {
                            printf("No songs found with the specified artist.\n");
                        } else {
                            printf("Songs by artist %s:\n", artist);

                            // Print all songs in the result list
                            Song *temp = filtered;
                            while (temp != NULL) {
                                printf("%s by %s (%s, %d)\n", temp->title, temp->artist, temp->genre, temp->year);
                                temp = temp->right;
                            }
                        }
                        break;
                    }

                    case 3: {
                        // Filter by genre
                        char genre[50];

                        printf("Enter genre to filter: ");
                        fgets(inputBuffer, sizeof(inputBuffer), stdin);
                        inputBuffer[strcspn(inputBuffer, "\n")] = '\0'; // Remove the newline character
                        strncpy(genre, inputBuffer, sizeof(genre));
                    
                        // Search for songs by genre and print them
                        Song* filtered = findSongsByGenre(root, genre);
                    
                        if (filtered == NULL) {
                            printf("No songs found with the specified genre.\n");
                        } else {
                            printf("Songs with genre %s:\n", genre);
                    
                            while (filtered != NULL) {
                                printf("%s by %s (%s, %d)\n", filtered->title, filtered->artist, filtered->genre, filtered->year);
                                filtered = filtered->right; // Move to the next song with the same genre
                            }
                        }
                        break;
                    }

                    case 4: {
                        // Filter by year
                        int year;

                        printf("Enter year to filter: ");
                        fgets(inputBuffer, sizeof(inputBuffer), stdin);
                        year = atoi(inputBuffer); // Convert the input to an integer

                        int count = 0;
                        Song* filtered = findSongsByYear(root, year, &count);

                        if (count == 0) {
                            printf("No songs found for the year.\n");
                        } else {
                            printf("Songs from %d:\n", year);

                            Song* current = filtered;
                            while (current != NULL) {
                                printf("%s by %s (%s, %d)\n", current->title, current->artist, current->genre, current->year);
                                current = current->right;
                            }
                        }

                        break;
                    }

                    case 5: {
                        // Back to main menu
                        break;
                    }
                    default: {
                        printf("Invalid choice in the Filter submenu\n");
                    }
                }
                break;
            }
            case 3: {
                // Delete a song
                char title[100];

                printf("Enter song title to delete: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0'; // Remove the newline character

                Song* beforeDeletion = findSongByTitle(root, title); // Check if the song exists before deletion

                if (beforeDeletion != NULL) {
                    root = deleteNode(root, title);
                    printf("Song deleted successfully\n");
                } else {
                    printf("Song with title '%s' not found in the playlist. Cannot delete.\n", title);
                }
                break;
            }


            case 4: {
                // Shuffle playlist
                if (root == NULL) {
                    printf("The playlist is empty. Cannot shuffle.\n");
                } else {
                    int size = getSize(root);
                    Song *shuffled[size];
                    int index = 0;
            
                    shufflePlaylist(root, shuffled, &index);
                    printShuffledPlaylist(shuffled, size);
                }
                break;
            }

            case 5: {
                // Find most common artist
                char mostCommonArtist[100];
                int maxCount = 0;

                if (root == NULL) {
                    printf("The playlist is empty. There are no songs to find the most common artist.\n");
                } else {
                    findMostCommonArtist(root, mostCommonArtist, &maxCount);
                    printf("Most common artist: %s (%d songs)\n", mostCommonArtist, maxCount);
                }
                break;
            }

            case 6: {
                // Find most common genre
                char mostCommonGenre[50];
                int maxCount = 0;

                if (root == NULL) {
                    printf("The playlist is empty. There are no songs to find the most common genre.\n");
                } else {
                    findMostCommonGenre(root, mostCommonGenre, &maxCount);
                    printf("Most common genre: %s (%d songs)\n", mostCommonGenre, maxCount);
                }
                break;
            }
            
            case 7: {
                // Print playlist
                inorder(root);
                break;
            }
            case 8: {
                // Exit
                printf("Exiting program...\n");
                return 0;
            }
            default: {
                printf("Invalid choice\n");
            }
        }
    }
    return 0;
}
