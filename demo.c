#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_INGREDIENTS 10
#define MAX_RECIPES 20
#define MAX_PANTRY 50
#define MAX_GROCERY 50


struct TrieNode {
    struct TrieNode* children[26];
    int isEndOfWord;
    int recipeIndex;
};


typedef struct {
    char name[MAX_NAME];
    char ingredients[MAX_INGREDIENTS][MAX_NAME];
    int quantities[MAX_INGREDIENTS];
    int numIngredients;
} Recipe;

// Pantry item structure
typedef struct {
    char name[MAX_NAME];
    int quantity;
} PantryItem;

// Grocery list structure
typedef struct {
    char items[MAX_GROCERY][MAX_NAME];
    int priorities[MAX_GROCERY];
    int size;
} GroceryList;

// Global variables
Recipe recipes[MAX_RECIPES];
int recipeCount = 0;
PantryItem pantry[MAX_PANTRY];
int pantryCount = 0;
GroceryList groceryList;
struct TrieNode* recipeRoot;

// Function to create new Trie node
struct TrieNode* createNode() {
    struct TrieNode* node = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    for (int i = 0; i < 26; i++) {
        node->children[i] = NULL;
    }
    node->isEndOfWord = 0;
    node->recipeIndex = -1;
    return node;
}

// Function to insert recipe into Trie
void insertRecipeInTrie(char* name, int index) {
    struct TrieNode* current = recipeRoot;
    while (*name) {
        int idx = tolower(*name) - 'a';
        if (!current->children[idx]) {
            current->children[idx] = createNode();
        }
        current = current->children[idx];
        name++;
    }
    current->isEndOfWord = 1;
    current->recipeIndex = index;
}

// Function to search recipe in Trie
int searchRecipe(char* name) {
    struct TrieNode* current = recipeRoot;
    while (*name) {
        int idx = tolower(*name) - 'a';
        if (!current->children[idx]) {
            return -1;
        }
        current = current->children[idx];
        name++;
    }
    return (current && current->isEndOfWord) ? current->recipeIndex : -1;
}

// Function to add new recipe
void addRecipe() {
    if (recipeCount >= MAX_RECIPES) {
        printf("Recipe book is full!\n");
        return;
    }

    Recipe* newRecipe = &recipes[recipeCount];
    printf("\nEnter recipe name: ");
    scanf(" %[^\n]s", newRecipe->name);
    
    printf("Enter number of ingredients (max %d): ", MAX_INGREDIENTS);
    scanf("%d", &newRecipe->numIngredients);
    
    if (newRecipe->numIngredients > MAX_INGREDIENTS) {
        printf("Too many ingredients! Maximum allowed is %d\n", MAX_INGREDIENTS);
        return;
    }
    
    printf("\nEnter ingredients and quantities:\n");
    for (int i = 0; i < newRecipe->numIngredients; i++) {
        printf("Ingredient %d name: ", i + 1);
        scanf(" %[^\n]s", newRecipe->ingredients[i]);
        printf("Quantity needed: ");
        scanf("%d", &newRecipe->quantities[i]);
    }
    
    insertRecipeInTrie(newRecipe->name, recipeCount);
    recipeCount++;
    printf("\nRecipe added successfully!\n");
}

// Function to add item to pantry
void addToPantry() {
    char itemName[MAX_NAME];
    int quantity;
    
    printf("\nEnter item name: ");
    scanf(" %[^\n]s", itemName);
    printf("Enter quantity: ");
    scanf("%d", &quantity);
    
    // Check if item already exists
    for (int i = 0; i < pantryCount; i++) {
        if (strcmp(pantry[i].name, itemName) == 0) {
            pantry[i].quantity += quantity;
            printf("Updated quantity of %s in pantry!\n", itemName);
            return;
        }
    }
    
    // Add new item if doesn't exist
    if (pantryCount < MAX_PANTRY) {
        strcpy(pantry[pantryCount].name, itemName);
        pantry[pantryCount].quantity = quantity;
        pantryCount++;
        printf("Item added to pantry!\n");
    } else {
        printf("Pantry is full!\n");
    }
}

// Function to add item to grocery list
void addToGroceryList(char* item, int priority) {
    // Check if item already exists
    for (int i = 0; i < groceryList.size; i++) {
        if (strcmp(groceryList.items[i], item) == 0) {
            if (priority > groceryList.priorities[i]) {
                groceryList.priorities[i] = priority;
            }
            return;
        }
    }
    
    if (groceryList.size < MAX_GROCERY) {
        strcpy(groceryList.items[groceryList.size], item);
        groceryList.priorities[groceryList.size] = priority;
        groceryList.size++;
    } else {
        printf("Grocery list is full!\n");
    }
}

// Function to display grocery list
void displayGroceryList() {
    if (groceryList.size == 0) {
        printf("\nGrocery list is empty!\n");
        return;
    }
    
    printf("\nGrocery List:\n");
    printf("Priority levels: 3=Urgent, 2=Medium, 1=Low\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < groceryList.size; i++) {
        printf("%d. %s (Priority: %d)\n", i+1, groceryList.items[i], groceryList.priorities[i]);
    }
}

// Function to display pantry
void displayPantry() {
    if (pantryCount == 0) {
        printf("\nPantry is empty!\n");
        return;
    }
    
    printf("\nCurrent Pantry Contents:\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < pantryCount; i++) {
        printf("%s: %d units\n", pantry[i].name, pantry[i].quantity);
    }
    printf("----------------------------------------\n");
}

// Function to check ingredient availability
int checkIngredientAvailability(char* ingredient, int requiredQuantity) {
    for (int i = 0; i < pantryCount; i++) {
        if (strcmp(pantry[i].name, ingredient) == 0) {
            return (pantry[i].quantity >= requiredQuantity);
        }
    }
    return 0;
}

// Function to update pantry quantity
void updatePantryQuantity(char* ingredient, int amount) {
    for (int i = 0; i < pantryCount; i++) {
        if (strcmp(pantry[i].name, ingredient) == 0) {
            pantry[i].quantity += amount;
            if (pantry[i].quantity < 0) {
                printf("Warning: %s quantity is negative!\n", ingredient);
            }
            return;
        }
    }
}

// Function to plan meal (check ingredients without using them)
void planMeal() {
    char recipeName[MAX_NAME];
    printf("\nEnter recipe name to plan: ");
    scanf(" %[^\n]s", recipeName);
    
    int recipeIndex = searchRecipe(recipeName);
    if (recipeIndex == -1) {
        printf("Recipe not found!\n");
        return;
    }
    
    Recipe* recipe = &recipes[recipeIndex];
    printf("\nChecking ingredients for %s:\n", recipe->name);
    printf("----------------------------------------\n");
    
    for (int i = 0; i < recipe->numIngredients; i++) {
        int available = 0;
        for (int j = 0; j < pantryCount; j++) {
            if (strcmp(recipe->ingredients[i], pantry[j].name) == 0) {
                available = pantry[j].quantity;
                break;
            }
        }
        
        printf("%s - Need: %d, Have: %d\n", 
               recipe->ingredients[i], 
               recipe->quantities[i], 
               available);
               
        if (available < recipe->quantities[i]) {
            printf("*** Need to buy more %s (Added to grocery list)\n", 
                   recipe->ingredients[i]);
            addToGroceryList(recipe->ingredients[i], 2);
        }
    }
    printf("----------------------------------------\n");
}

// Function to create meal (use ingredients)
void createMeal() {
    char recipeName[MAX_NAME];
    printf("\nEnter recipe name to create: ");
    scanf(" %[^\n]s", recipeName);
    
    int recipeIndex = searchRecipe(recipeName);
    if (recipeIndex == -1) {
        printf("Recipe not found!\n");
        return;
    }
    
    Recipe* recipe = &recipes[recipeIndex];
    printf("\nChecking ingredients for %s:\n", recipe->name);
    
    // Check if all ingredients are available
    int canCook = 1;
    for (int i = 0; i < recipe->numIngredients; i++) {
        if (!checkIngredientAvailability(recipe->ingredients[i], recipe->quantities[i])) {
            printf("Insufficient quantity of %s\n", recipe->ingredients[i]);
            canCook = 0;
            addToGroceryList(recipe->ingredients[i], 3);
        }
    }
    
    if (!canCook) {
        printf("\nCannot create meal - missing ingredients added to grocery list\n");
        return;
    }
    
    // Use ingredients
    printf("\nCreating meal %s...\n", recipe->name);
    for (int i = 0; i < recipe->numIngredients; i++) {
        updatePantryQuantity(recipe->ingredients[i], -recipe->quantities[i]);
        printf("Used %d units of %s\n", recipe->quantities[i], recipe->ingredients[i]);
    }
    
    printf("\nMeal created successfully!\n");
}

// Initialize system with sample data+
void initializeSystem() {
    recipeRoot = createNode();
    groceryList.size = 0;
    
    // Sample recipe: Pasta
    strcpy(recipes[recipeCount].name, "Pasta");
    recipes[recipeCount].numIngredients = 3;
    
    strcpy(recipes[recipeCount].ingredients[0], "pasta");
    recipes[recipeCount].quantities[0] = 2;
    
    strcpy(recipes[recipeCount].ingredients[1], "tomato sauce");
    recipes[recipeCount].quantities[1] = 1;
    
    strcpy(recipes[recipeCount].ingredients[2], "cheese");
    recipes[recipeCount].quantities[2] = 1;
    
    insertRecipeInTrie(recipes[recipeCount].name, recipeCount);
    recipeCount++;
    
    // Sample recipe: Sandwich
    strcpy(recipes[recipeCount].name, "Sandwich");
    recipes[recipeCount].numIngredients = 3;
    
    strcpy(recipes[recipeCount].ingredients[0], "bread");
    recipes[recipeCount].quantities[0] = 2;
    
    strcpy(recipes[recipeCount].ingredients[1], "cheese");
    recipes[recipeCount].quantities[1] = 1;
    
    strcpy(recipes[recipeCount].ingredients[2], "tomato");
    recipes[recipeCount].quantities[2] = 1;
    
    insertRecipeInTrie(recipes[recipeCount].name, recipeCount);
    recipeCount++;
    
    // Sample pantry items
    strcpy(pantry[pantryCount].name, "pasta");
    pantry[pantryCount].quantity = 4;
    pantryCount++;
    
    strcpy(pantry[pantryCount].name, "bread");
    pantry[pantryCount].quantity = 6;
    pantryCount++;
    
    strcpy(pantry[pantryCount].name, "cheese");
    pantry[pantryCount].quantity = 3;
    pantryCount++;
    
    strcpy(pantry[pantryCount].name, "tomato sauce");
    pantry[pantryCount].quantity = 2;
    pantryCount++;
    
    strcpy(pantry[pantryCount].name, "tomato");
    pantry[pantryCount].quantity = 4;
    pantryCount++;
}

int main() {
    initializeSystem();
    int choice;
    
    while(1)
    {
        printf("\nMeal Planner and Grocery Manager");
        printf("\n================================");
        printf("\n1. Add Recipe");
        printf("\n2. Add to Pantry");
        printf("\n3. Plan Meal");
        printf("\n4. Create Meal");
        printf("\n5. View Grocery List");
        printf("\n6. View Pantry");
        printf("\n7. Exit");
        printf("\n================================");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                addRecipe();
                break;
            case 2:
                addToPantry();
                break;
            case 3:
                planMeal();
                break;
            case 4:
                createMeal();
                break;
            case 5:
                displayGroceryList();
                break;
            case 6:
                displayPantry();
                break;
            case 7:
                printf("\nThank you for using Meal Planner!\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
        if(choice==7)
        break; 
    }
    return 0;
}