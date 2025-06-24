# LangITB - A Beginner-Friendly Programming Language

LangITB is a simple algorithmic notation that serves as the idea of a programming language Through this program, you can test your algorithmic notation.

## ✨ Features (What Can You Do?)

- 📝 **Declare variables** - Store numbers, text, and true/false values
- 🔄 **Create loops** - Repeat actions multiple times
- ❓ **Make decisions** - Do different things based on conditions
- 💬 **Get input/output** - Talk to the user
- 📝 **Add comments** - Leave notes for yourself
- 🏗️ **Organize code** - Use indentation like Python

## 📚 Learning the Syntax (How to Write Code)

### Step 1: Declare Your Variables
First, tell the computer what kind of data you'll use:

```notal
KAMUS
age : integer          # A whole number
name : string          # Text
height : float         # Decimal number
is_student : bool      # True or false
grade : char           # Single letter
```

**What each type means:**
- `integer` → Whole numbers (1, 2, 100, -5)
- `string` → Text ("Hello", "My name is...")
- `float` → Decimal numbers (3.14, 2.5, -1.8)
- `bool` → True or false values
- `char` → Single letters ('A', 'x', '!')

### Step 2: Write Your Program
After declaring variables, start your actual program:

```notal
ALGORITMA
# Your program starts here!
```

### Step 3: Basic Operations

#### Storing Values (Assignment)
```notal
age <- 20                    # Store number 20 in age
name <- "Alice"             # Store text "Alice" in name
is_student <- true          # Store true in is_student
```

#### Talking to Users (Input/Output)
```notal
input(age)                  # Ask user to enter their age
output("Hello World!")      # Show "Hello World!" on screen
output(age)                 # Show the value of age
```

#### Making Decisions (If-Then-Else)
```notal
if (age >= 18) then
    output("You are an adult")
else
    output("You are a minor")
```

#### Repeating Actions (Loops)

**Count from 1 to 10:**
```notal
i traversal [1..10]:
    output(i)
```

**Keep asking until user enters 0:**
```notal
while (number != 0) do:
    input(number)
    output("You entered: ")
    output(number)
```

**Do something at least once:**
```notal
do:
    output("Enter a number: ")
    input(number)
while (number < 0)
```

**Repeat until condition is met:**
```notal
repeat:
    output("Guess the number: ")
    input(guess)
until (guess == 42)
```

#### Adding Comments (Notes to Yourself)
```notal
# This is a comment - the computer ignores this line
output("Hello")  {This is also a comment}

{
This is a 
multi-line comment
for longer explanations
}
```

## 🎯 Complete Example for Beginners

Let's create a simple program that asks for your name and age, then tells you something about yourself:

```notal
KAMUS
user_name : string
user_age : integer
birth_year : integer

ALGORITMA

# Ask for user information
output("What is your name? ")
input(user_name)
output("How old are you? ")
input(user_age)

# Calculate birth year
birth_year <- 2024 - user_age

# Greet the user
output("Hello, ")
output(user_name)
output("!")
output("You were born around ")
output(birth_year)

# Check if they can vote
if (user_age >= 18) then
    output("You can vote!")
else
    output("You cannot vote yet.")

# Count down their age
output("Counting down from your age:")
i traversal [user_age..1]:
    output(i)
output("Done!")
```

## 🛠️ Installation & Usage (How to Get Started)

### For Windows Users

#### Step 1: Install a C++ Compiler
1. Download **MinGW-w64** from [winlibs.com](https://winlibs.com/)
2. Extract it to `C:\mingw64`
3. Add `C:\mingw64\bin` to your system PATH

#### Step 2: Get LangITB
1. Download the `Compiler.cpp` file
2. Open Command Prompt or PowerShell
3. Navigate to the folder containing `Compiler.cpp`

#### Step 3: Build the Compiler
```cmd
g++ -o compiler.exe Compiler.cpp
```

#### Step 4: Write Your First Program
1. Create a file called `my_program.notal`
2. Write your LangITB code in it
3. Save the file

#### Step 5: Run Your Program
```cmd
compiler.exe my_program.notal
```

### For Mac/Linux Users

#### Step 1: Install C++ Compiler
**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential
```

**macOS:**
```bash
xcode-select --install
```

#### Step 2: Get LangITB
```bash
# Download or clone the repository
git clone <repository-url>
cd LangITB
```

#### Step 3: Build the Compiler
```bash
g++ -o compiler Compiler.cpp
chmod +x compiler
```

#### Step 4: Run Your Program
```bash
./compiler my_program.notal
```

## 🎓 Tutorial: Your First Program

Let's create a simple number guessing game step by step!

### Step 1: Create the file
Create a new file called `guessing_game.notal`

### Step 2: Set up variables
```notal
KAMUS
secret_number : integer
user_guess : integer
tries : integer
```

### Step 3: Write the game logic
```notal
ALGORITMA

secret_number <- 7
tries <- 0

output("Welcome to the Number Guessing Game!")
output("I'm thinking of a number between 1 and 10")

repeat:
    tries <- tries + 1
    output("Enter your guess: ")
    input(user_guess)
    
    if (user_guess == secret_number) then
        output("Congratulations! You guessed it!")
        output("It took you ")
        output(tries)
        output(" tries.")
    else
        if (user_guess < secret_number) then
            output("Too low! Try again.")
        else
            output("Too high! Try again.")
until (user_guess == secret_number)

output("Thanks for playing!")
```

### Step 4: Run it!
```bash
./compiler guessing_game.notal
```

## 🔧 Troubleshooting (When Things Go Wrong)

### "Command not found" or "g++ not recognized"
**Problem:** Your computer can't find the C++ compiler
**Solution:** 
- Windows: Make sure MinGW is installed and added to PATH
- Mac/Linux: Install build tools as shown above

### "Permission denied"
**Problem:** (Linux/Mac only) The compiler file isn't executable
**Solution:** 
```bash
chmod +x compiler
```

### "Compilation failed"
**Problem:** There's an error in your .notal file
**Solutions:**
- Check your spelling (is it `output` not `print`?)
- Make sure you have `KAMUS` and `ALGORITMA` sections
- Make sure every `if` has a matching `else` or proper structure


## 📖 Language Reference Card

| What You Want to Do |           LangITB Syntax           |         Example          |
|---------------------|------------------------------------|--------------------------|
| Store a number      | `variable <- value`                | `age <- 25`              |
| Store text          | `variable <- "text"`               | `name <- "John"`         |
| Get user input      | `input(variable)`                  | `input(age)`             |
| Show output         | `output(expression)`               | `output("Hello!")`       |
| Make decision       | `if (condition) then ... else ...` | `if (age >= 18) then...` |
| Count loop          | `var traversal [start..end]:`      | `i traversal [1..10]:`   |
| While loop          | `while (condition) do:`            | `while (x < 10) do:`     |
| Comment             | `# comment`                        | `# This is a note`       |

## 🤝 Need Help?

- **Stuck on syntax?** Check the examples above
- **Program not working?** Look at the troubleshooting section
- **Want to learn more?** Try the practice exercises
- **Found a bug?** Create an issue on GitHub

## 👨‍💻 About This Project

LangITB was created to help students learn how to express their idea on a more universal syntax. Through algorithmic notation, programmers does not need to worry about syntax and other cons of a programming language, and thus could focus on the logic itself.

---