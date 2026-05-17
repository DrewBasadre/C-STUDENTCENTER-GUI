#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define DATA_FILE "student_records.dat"
#define REPORT_FILE "student_report.txt"
#define NAME_SIZE 60
#define COURSE_SIZE 30
#define SECTION_SIZE 20
#define SUBJECT_COUNT 5

typedef struct {
    int id;
    char name[NAME_SIZE];
    char course[COURSE_SIZE];
    char section[SECTION_SIZE];
    int yearLevel;
    float grades[SUBJECT_COUNT];
} Student;

const char *SUBJECTS[SUBJECT_COUNT] = {
    "Programming",
    "Mathematics",
    "English",
    "Science",
    "Filipino"
};

void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD position;
    position.X = (SHORT)(x - 1);
    position.Y = (SHORT)(y - 1);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
#else
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
#endif
}

void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

void drawBox(int x, int y, int width, int height) {
    int row, col;

    gotoxy(x, y);
    putchar('+');
    for (col = 0; col < width - 2; col++) {
        putchar('-');
    }
    putchar('+');

    for (row = 1; row < height - 1; row++) {
        gotoxy(x, y + row);
        putchar('|');
        gotoxy(x + width - 1, y + row);
        putchar('|');
    }

    gotoxy(x, y + height - 1);
    putchar('+');
    for (col = 0; col < width - 2; col++) {
        putchar('-');
    }
    putchar('+');
}

void centerText(int y, const char *text) {
    int x = (80 - (int)strlen(text)) / 2;
    if (x < 1) {
        x = 1;
    }
    gotoxy(x, y);
    printf("%s", text);
}

void printHeader(const char *title) {
    clearScreen();
    drawBox(2, 1, 78, 5);
    centerText(2, "STUDENT CENTER FILE HANDLING SYSTEM");
    centerText(3, title);
    gotoxy(4, 5);
    printf("Data File: %s", DATA_FILE);
}

void pauseScreen(void) {
    char buffer[8];

    gotoxy(4, 24);
    printf("Press ENTER to continue...");
    fgets(buffer, sizeof(buffer), stdin);
}

void trimNewline(char *text) {
    size_t length = strlen(text);

    if (length > 0 && text[length - 1] == '\n') {
        text[length - 1] = '\0';
    }
}

void readLine(int x, int y, const char *label, char *target, int size) {
    gotoxy(x, y);
    printf("%-18s: ", label);
    fgets(target, size, stdin);
    trimNewline(target);
}

int readInt(int x, int y, const char *label, int minimum, int maximum) {
    char buffer[80];
    int value;
    char extra;

    while (1) {
        gotoxy(x, y);
        printf("%-18s: %-40s", label, "");
        gotoxy(x + 20, y);
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%d %c", &value, &extra) == 1 &&
            value >= minimum && value <= maximum) {
            return value;
        }

        gotoxy(x, y + 1);
        printf("Invalid input. Enter a number from %d to %d.          ", minimum, maximum);
    }
}

float readFloat(int x, int y, const char *label, float minimum, float maximum) {
    char buffer[80];
    float value;
    char extra;

    while (1) {
        gotoxy(x, y);
        printf("%-18s: %-40s", label, "");
        gotoxy(x + 20, y);
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%f %c", &value, &extra) == 1 &&
            value >= minimum && value <= maximum) {
            return value;
        }

        gotoxy(x, y + 1);
        printf("Invalid input. Enter a grade from %.0f to %.0f.       ", minimum, maximum);
    }
}

char readChoice(int x, int y, const char *label) {
    char buffer[20];

    gotoxy(x, y);
    printf("%s", label);
    fgets(buffer, sizeof(buffer), stdin);
    return (char)toupper((unsigned char)buffer[0]);
}

float computeAverage(const Student *student) {
    float total = 0.0f;
    int i;

    for (i = 0; i < SUBJECT_COUNT; i++) {
        total += student->grades[i];
    }

    return total / SUBJECT_COUNT;
}

const char *getRemarks(float average) {
    if (average >= 75.0f) {
        return "PASSED";
    }

    return "FAILED";
}

int fileExists(void) {
    FILE *file = fopen(DATA_FILE, "rb");

    if (file == NULL) {
        return 0;
    }

    fclose(file);
    return 1;
}

long findRecordPositionById(int id, Student *foundStudent) {
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;
    long position = -1;

    if (file == NULL) {
        return -1;
    }

    while (fread(&student, sizeof(Student), 1, file) == 1) {
        if (student.id == id) {
            position = ftell(file) - (long)sizeof(Student);
            if (foundStudent != NULL) {
                *foundStudent = student;
            }
            break;
        }
    }

    fclose(file);
    return position;
}

void printStudentCard(int x, int y, const Student *student) {
    int i;
    float average = computeAverage(student);

    drawBox(x, y, 72, 12);
    gotoxy(x + 3, y + 1);
    printf("Student ID : %-10d Year Level : %d", student->id, student->yearLevel);
    gotoxy(x + 3, y + 2);
    printf("Name       : %s", student->name);
    gotoxy(x + 3, y + 3);
    printf("Course     : %-20s Section : %s", student->course, student->section);
    gotoxy(x + 3, y + 5);
    printf("Grades");

    for (i = 0; i < SUBJECT_COUNT; i++) {
        gotoxy(x + 6, y + 6 + i);
        printf("%-14s : %6.2f", SUBJECTS[i], student->grades[i]);
    }

    gotoxy(x + 40, y + 6);
    printf("Average : %.2f", average);
    gotoxy(x + 40, y + 7);
    printf("Remarks : %s", getRemarks(average));
}

Student inputStudent(int titleRow) {
    Student student;
    int i;
    int row = titleRow;

    memset(&student, 0, sizeof(Student));

    student.id = readInt(7, row, "Student ID", 1, 999999);
    readLine(7, row + 2, "Full Name", student.name, NAME_SIZE);
    readLine(7, row + 3, "Course", student.course, COURSE_SIZE);
    readLine(7, row + 4, "Section", student.section, SECTION_SIZE);
    student.yearLevel = readInt(7, row + 5, "Year Level", 1, 6);

    gotoxy(7, row + 7);
    printf("Enter Grades");
    for (i = 0; i < SUBJECT_COUNT; i++) {
        student.grades[i] = readFloat(10, row + 8 + i, SUBJECTS[i], 0.0f, 100.0f);
    }

    return student;
}

void createFile(void) {
    FILE *file;
    char choice;

    printHeader("1. CREATION OF FILE");
    drawBox(5, 8, 70, 9);

    if (fileExists()) {
        gotoxy(8, 10);
        printf("A student data file already exists.");
        gotoxy(8, 11);
        printf("Creating a new file will erase all existing records.");
        choice = readChoice(8, 13, "Continue and recreate file? [Y/N]: ");

        if (choice != 'Y') {
            gotoxy(8, 15);
            printf("Operation cancelled. Existing file was kept.");
            pauseScreen();
            return;
        }
    }

    file = fopen(DATA_FILE, "wb");
    if (file == NULL) {
        gotoxy(8, 15);
        printf("Unable to create file.");
        pauseScreen();
        return;
    }

    fclose(file);
    gotoxy(8, 15);
    printf("File created successfully: %s", DATA_FILE);
    pauseScreen();
}

void addRecord(void) {
    FILE *file;
    Student student;

    printHeader("2. ADDING OF RECORDS");
    drawBox(5, 7, 70, 17);

    student = inputStudent(9);
    if (findRecordPositionById(student.id, NULL) != -1) {
        gotoxy(7, 22);
        printf("Student ID already exists. Record was not added.");
        pauseScreen();
        return;
    }

    file = fopen(DATA_FILE, "ab");
    if (file == NULL) {
        gotoxy(7, 22);
        printf("Unable to open data file. Create the file first.");
        pauseScreen();
        return;
    }

    fwrite(&student, sizeof(Student), 1, file);
    fclose(file);

    gotoxy(7, 22);
    printf("Record added successfully.");
    pauseScreen();
}

void searchRecord(void) {
    Student student;
    int id;

    printHeader("3. SEARCHING OF INDIVIDUAL RECORDS");
    drawBox(5, 7, 70, 5);

    id = readInt(8, 9, "Student ID", 1, 999999);

    if (findRecordPositionById(id, &student) == -1) {
        gotoxy(8, 13);
        printf("No record found for Student ID %d.", id);
    } else {
        printStudentCard(5, 12, &student);
    }

    pauseScreen();
}

void updateRecord(void) {
    FILE *file;
    Student student;
    long position;
    int id;
    int i;
    char choice;

    printHeader("4. UPDATING OF RECORDS - ARRAY OF GRADES");
    drawBox(5, 7, 70, 5);

    id = readInt(8, 9, "Student ID", 1, 999999);
    position = findRecordPositionById(id, &student);

    if (position == -1) {
        gotoxy(8, 13);
        printf("No record found for Student ID %d.", id);
        pauseScreen();
        return;
    }

    clearScreen();
    printHeader("CURRENT RECORD");
    printStudentCard(5, 7, &student);

    choice = readChoice(7, 20, "Update student profile also? [Y/N]: ");
    if (choice == 'Y') {
        clearScreen();
        printHeader("UPDATE PROFILE");
        drawBox(5, 7, 70, 9);
        gotoxy(8, 9);
        printf("Updating profile for Student ID %d", student.id);
        readLine(8, 11, "Full Name", student.name, NAME_SIZE);
        readLine(8, 12, "Course", student.course, COURSE_SIZE);
        readLine(8, 13, "Section", student.section, SECTION_SIZE);
        student.yearLevel = readInt(8, 14, "Year Level", 1, 6);
    }

    clearScreen();
    printHeader("UPDATE GRADES");
    drawBox(5, 7, 70, 12);
    gotoxy(8, 9);
    printf("Updating grades for %s", student.name);

    for (i = 0; i < SUBJECT_COUNT; i++) {
        student.grades[i] = readFloat(8, 11 + i, SUBJECTS[i], 0.0f, 100.0f);
    }

    file = fopen(DATA_FILE, "rb+");
    if (file == NULL) {
        gotoxy(8, 20);
        printf("Unable to open data file.");
        pauseScreen();
        return;
    }

    fseek(file, position, SEEK_SET);
    fwrite(&student, sizeof(Student), 1, file);
    fclose(file);

    gotoxy(8, 20);
    printf("Record updated successfully.");
    pauseScreen();
}

void deleteRecord(void) {
    FILE *source;
    FILE *temp;
    Student student;
    int id;
    int found = 0;
    char choice;

    printHeader("5. DELETION OF RECORDS");
    drawBox(5, 7, 70, 5);

    id = readInt(8, 9, "Student ID", 1, 999999);

    if (findRecordPositionById(id, &student) == -1) {
        gotoxy(8, 13);
        printf("No record found for Student ID %d.", id);
        pauseScreen();
        return;
    }

    printStudentCard(5, 11, &student);
    choice = readChoice(7, 23, "Delete this record? [Y/N]: ");
    if (choice != 'Y') {
        gotoxy(7, 23);
        printf("Deletion cancelled.");
        pauseScreen();
        return;
    }

    source = fopen(DATA_FILE, "rb");
    temp = fopen("student_records.tmp", "wb");

    if (source == NULL || temp == NULL) {
        if (source != NULL) {
            fclose(source);
        }
        if (temp != NULL) {
            fclose(temp);
        }
        gotoxy(7, 23);
        printf("Unable to process data file.");
        pauseScreen();
        return;
    }

    while (fread(&student, sizeof(Student), 1, source) == 1) {
        if (student.id == id) {
            found = 1;
        } else {
            fwrite(&student, sizeof(Student), 1, temp);
        }
    }

    fclose(source);
    fclose(temp);

    remove(DATA_FILE);
    rename("student_records.tmp", DATA_FILE);

    gotoxy(7, 23);
    if (found) {
        printf("Record deleted successfully.");
    } else {
        printf("Record was not deleted.");
    }
    pauseScreen();
}

void reportGeneration(void) {
    FILE *data;
    FILE *report;
    Student student;
    int count = 0;
    float classTotal = 0.0f;
    float highest = -1.0f;
    float lowest = 101.0f;
    char topStudent[NAME_SIZE] = "";
    char lowStudent[NAME_SIZE] = "";

    printHeader("6. REPORT GENERATION");

    data = fopen(DATA_FILE, "rb");
    if (data == NULL) {
        drawBox(5, 8, 70, 5);
        gotoxy(8, 10);
        printf("Unable to open data file. Create or add records first.");
        pauseScreen();
        return;
    }

    report = fopen(REPORT_FILE, "w");
    if (report == NULL) {
        fclose(data);
        drawBox(5, 8, 70, 5);
        gotoxy(8, 10);
        printf("Unable to create report file.");
        pauseScreen();
        return;
    }

    fprintf(report, "STUDENT CENTER REPORT\n");
    fprintf(report, "Data File: %s\n\n", DATA_FILE);
    fprintf(report, "%-8s %-24s %-12s %-8s %-9s %-8s\n",
            "ID", "Name", "Course", "Year", "Average", "Remarks");
    fprintf(report, "------------------------------------------------------------------------\n");

    drawBox(2, 7, 78, 15);
    gotoxy(4, 8);
    printf("%-8s %-24s %-12s %-6s %-9s %-8s",
           "ID", "Name", "Course", "Year", "Average", "Remarks");
    gotoxy(4, 9);
    printf("----------------------------------------------------------------------");

    while (fread(&student, sizeof(Student), 1, data) == 1) {
        float average = computeAverage(&student);

        count++;
        classTotal += average;

        if (average > highest) {
            highest = average;
            strncpy(topStudent, student.name, NAME_SIZE - 1);
            topStudent[NAME_SIZE - 1] = '\0';
        }

        if (average < lowest) {
            lowest = average;
            strncpy(lowStudent, student.name, NAME_SIZE - 1);
            lowStudent[NAME_SIZE - 1] = '\0';
        }

        fprintf(report, "%-8d %-24s %-12s %-8d %-9.2f %-8s\n",
                student.id,
                student.name,
                student.course,
                student.yearLevel,
                average,
                getRemarks(average));

        if (count <= 10) {
            gotoxy(4, 9 + count);
            printf("%-8d %-24.24s %-12.12s %-6d %-9.2f %-8s",
                   student.id,
                   student.name,
                   student.course,
                   student.yearLevel,
                   average,
                   getRemarks(average));
        }
    }

    if (count > 10) {
        gotoxy(4, 21);
        printf("More records are included in %s.", REPORT_FILE);
    }

    fprintf(report, "\nTotal Students : %d\n", count);
    if (count > 0) {
        fprintf(report, "Class Average  : %.2f\n", classTotal / count);
        fprintf(report, "Highest Average: %.2f - %s\n", highest, topStudent);
        fprintf(report, "Lowest Average : %.2f - %s\n", lowest, lowStudent);
    }

    fclose(data);
    fclose(report);

    drawBox(5, 21, 70, 3);
    gotoxy(8, 22);
    printf("Report generated successfully: %s", REPORT_FILE);
    gotoxy(8, 23);
    printf("Total students: %d", count);
    if (count > 0) {
        gotoxy(39, 23);
        printf("Class average: %.2f", classTotal / count);
    }

    pauseScreen();
}

void showMenu(void) {
    printHeader("MAIN MENU");
    drawBox(18, 7, 45, 14);

    gotoxy(24, 9);
    printf("[1] Creation of File");
    gotoxy(24, 10);
    printf("[2] Adding of Records");
    gotoxy(24, 11);
    printf("[3] Searching of Individual Records");
    gotoxy(24, 12);
    printf("[4] Updating of Records");
    gotoxy(24, 13);
    printf("[5] Deletion of Records");
    gotoxy(24, 14);
    printf("[6] Report Generation");
    gotoxy(24, 15);
    printf("[0] Exit");
    gotoxy(24, 18);
    printf("Select option: ");
}

int main(void) {
    int choice;

    do {
        showMenu();
        choice = readInt(24, 18, "Select option", 0, 6);

        switch (choice) {
            case 1:
                createFile();
                break;
            case 2:
                addRecord();
                break;
            case 3:
                searchRecord();
                break;
            case 4:
                updateRecord();
                break;
            case 5:
                deleteRecord();
                break;
            case 6:
                reportGeneration();
                break;
            case 0:
                printHeader("GOODBYE");
                centerText(11, "Thank you for using the Student Center System.");
                centerText(13, "Program closed.");
                gotoxy(1, 24);
                break;
        }
    } while (choice != 0);

    return 0;
}
