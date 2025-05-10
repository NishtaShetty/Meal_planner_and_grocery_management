// Constants
const MAX_RECIPES = 20;
const MAX_INGREDIENTS = 10;
const MAX_PANTRY = 50;

// Data structures
let recipes = [];
let pantry = [];
let groceryList = [];

// Initialize with sample data
function initializeData() {
    // Sample recipe: Pasta
    recipes.push({
        name: "Pasta",
        ingredients: [
            { name: "pasta", quantity: 2 },
            { name: "tomato sauce", quantity: 1 },
            { name: "cheese", quantity: 1 }
        ]
    });

    // Sample recipe: Sandwich
    recipes.push({
        name: "Sandwich",
        ingredients: [
            { name: "bread", quantity: 2 },
            { name: "cheese", quantity: 1 },
            { name: "tomato", quantity: 1 }
        ]
    });

    // Sample pantry items
    pantry.push({ name: "pasta", quantity: 4 });
    pantry.push({ name: "bread", quantity: 6 });
    pantry.push({ name: "cheese", quantity: 3 });
    pantry.push({ name: "tomato sauce", quantity: 2 });
    pantry.push({ name: "tomato", quantity: 4 });

    updateRecipeSelects();
    updatePantryDisplay();
}

// UI Functions
function showSection(sectionId) {
    // Hide all sections
    document.querySelectorAll('.program-section').forEach(section => {
        section.classList.add('hidden');
    });
    // Show selected section
    document.getElementById(sectionId).classList.remove('hidden');
}

function addIngredientField() {
    const ingredientsList = document.getElementById('ingredients-list');
    const ingredientCount = ingredientsList.children.length;
    
    if (ingredientCount < MAX_INGREDIENTS) {
        const newEntry = document.createElement('div');
        newEntry.className = 'ingredient-entry';
        newEntry.innerHTML = `
            <input type="text" placeholder="Ingredient Name" required>
            <input type="number" placeholder="Quantity" required min="1">
            <button type="button" onclick="removeIngredientField(this)">-</button>
        `;
        ingredientsList.appendChild(newEntry);
    } else {
        alert(`Maximum ${MAX_INGREDIENTS} ingredients allowed!`);
    }
}

function removeIngredientField(button) {
    button.parentElement.remove();
}

// Recipe Functions
function addRecipe(event) {
    event.preventDefault();
    
    if (recipes.length >= MAX_RECIPES) {
        alert('Recipe book is full!');
        return;
    }

    const form = event.target;
    const recipeName = form.querySelector('input[type="text"]').value;
    const ingredients = [];
    
    const ingredientEntries = form.querySelectorAll('.ingredient-entry');
    ingredientEntries.forEach(entry => {
        ingredients.push({
            name: entry.querySelector('input[type="text"]').value.toLowerCase(),
            quantity: parseInt(entry.querySelector('input[type="number"]').value)
        });
    });

    recipes.push({ name: recipeName, ingredients: ingredients });
    updateRecipeSelects();
    form.reset();
    alert('Recipe added successfully!');
}

// Pantry Functions
function addToPantry(event) {
    event.preventDefault();
    
    if (pantry.length >= MAX_PANTRY) {
        alert('Pantry is full!');
        return;
    }

    const form = event.target;
    const itemName = form.querySelector('input[type="text"]').value.toLowerCase();
    const quantity = parseInt(form.querySelector('input[type="number"]').value);

    const existingItem = pantry.find(item => item.name === itemName);
    if (existingItem) {
        existingItem.quantity += quantity;
    } else {
        pantry.push({ name: itemName, quantity: quantity });
    }

    updatePantryDisplay();
    form.reset();
    alert('Item added to pantry!');
}

// Meal Planning Functions
function planMeal(event) {
    event.preventDefault();
    
    const recipeName = document.getElementById('recipe-select').value;
    const recipe = recipes.find(r => r.name === recipeName);
    const results = document.getElementById('ingredient-check-results');
    
    if (!recipe) {
        results.innerHTML = '<p>Recipe not found!</p>';
        return;
    }

    let checkResults = '<h4>Ingredient Check:</h4><ul>';
    recipe.ingredients.forEach(ingredient => {
        const pantryItem = pantry.find(item => item.name === ingredient.name);
        const available = pantryItem ? pantryItem.quantity : 0;
        const status = available >= ingredient.quantity ? '✅' : '❌';
        
        checkResults += `<li>${status} ${ingredient.name}: Need ${ingredient.quantity}, Have ${available}</li>`;
        
        if (available < ingredient.quantity) {
            addToGroceryList(ingredient.name, 2);
        }
    });
    checkResults += '</ul>';
    results.innerHTML = checkResults;
    updateGroceryDisplay();
}

// Meal Creation Functions
function createMeal(event) {
    event.preventDefault();
    
    const recipeName = document.getElementById('recipe-select-create').value;
    const recipe = recipes.find(r => r.name === recipeName);
    const results = document.getElementById('meal-creation-results');
    
    if (!recipe) {
        results.innerHTML = '<p>Recipe not found!</p>';
        return;
    }

    // Check if all ingredients are available
    let canCook = true;
    let resultMessage = '<h4>Creating Meal:</h4><ul>';
    
    recipe.ingredients.forEach(ingredient => {
        const pantryItem = pantry.find(item => item.name === ingredient.name);
        if (!pantryItem || pantryItem.quantity < ingredient.quantity) {
            canCook = false;
            resultMessage += `<li>❌ Insufficient ${ingredient.name}</li>`;
            addToGroceryList(ingredient.name, 3);
        }
    });

    if (!canCook) {
        resultMessage += '</ul><p>Cannot create meal - missing ingredients added to grocery list</p>';
        results.innerHTML = resultMessage;
        updateGroceryDisplay();
        return;
    }

    // Use ingredients
    recipe.ingredients.forEach(ingredient => {
        const pantryItem = pantry.find(item => item.name === ingredient.name);
        pantryItem.quantity -= ingredient.quantity;
        resultMessage += `<li>✅ Used ${ingredient.quantity} ${ingredient.name}</li>`;
    });

    resultMessage += '</ul><p>Meal created successfully!</p>';
    results.innerHTML = resultMessage;
    updatePantryDisplay();
}

// Grocery List Functions
function addToGroceryList(itemName, priority) {
    const existingItem = groceryList.find(item => item.name === itemName);
    if (existingItem) {
        existingItem.priority = Math.max(existingItem.priority, priority);
    } else {
        groceryList.push({ name: itemName, priority: priority });
    }
    updateGroceryDisplay();
}

// Display Update Functions
function updateRecipeSelects() {
    const recipeSelects = document.querySelectorAll('#recipe-select, #recipe-select-create');
    const options = recipes.map(recipe => 
        `<option value="${recipe.name}">${recipe.name}</option>`
    ).join('');
    
    recipeSelects.forEach(select => {
        select.innerHTML = '<option value="">Select Recipe</option>' + options;
    });
}

function updatePantryDisplay() {
    const pantryList = document.getElementById('pantry-items');
    pantryList.innerHTML = pantry.map(item =>
        `<li>${item.name}: ${item.quantity} units</li>`
    ).join('');
}

function updateGroceryDisplay() {
    const groceryItems = document.getElementById('grocery-items');
    const sortedList = [...groceryList].sort((a, b) => b.priority - a.priority);
    
    groceryItems.innerHTML = sortedList.map(item => {
        const priorityClass = item.priority === 3 ? 'priority-high' : 
                            item.priority === 2 ? 'priority-medium' : 
                            'priority-low';
        return `<li class="${priorityClass}">${item.name} (Priority: ${item.priority})</li>`;
    }).join('');
}

// Event Listeners
document.addEventListener('DOMContentLoaded', () => {
    initializeData();
    
    document.getElementById('recipe-form').addEventListener('submit', addRecipe);
    document.getElementById('pantry-form').addEventListener('submit', addToPantry);
    document.getElementById('plan-meal-form').addEventListener('submit', planMeal);
    document.getElementById('create-meal-form').addEventListener('submit', createMeal);
}); 