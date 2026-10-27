#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STR_LEN   50
#define EVID_LEN  100
#define DATA_FILE "cases.txt"


typedef struct Case {
    int  id;
    char crimeType[STR_LEN];
    char location[STR_LEN];
    char date[STR_LEN];
    char evidence[EVID_LEN];
    char officer[STR_LEN];
    char status[STR_LEN];
    struct Case *next;
} Case;

Case *head = NULL;                    


void readLine(const char *prompt, char *buf, int size) {
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
        buf[strcspn(buf, "\n")] = '\0';
        for (char *p = buf; *p; p++)    
            if (*p == '|') *p = '/';
        if (buf[0] != '\0') return;     
        printf("  Field cannot be empty.\n");
    }
}

int readInt(const char *prompt);

int validDate(const char *d) {           
    int dd, mm, yy;
    if (strlen(d) != 10 || d[2] != '-' || d[5] != '-') return 0;
    if (sscanf(d, "%d-%d-%d", &dd, &mm, &yy) != 3) return 0;
    return dd >= 1 && dd <= 31 && mm >= 1 && mm <= 12 && yy >= 1900 && yy <= 2100;
}

void readDate(const char *prompt, char *buf) {
    while (1) {
        readLine(prompt, buf, STR_LEN);
        if (validDate(buf)) return;
        printf("  Use format DD-MM-YYYY (e.g. 21-10-2026).\n");
    }
}

void readStatus(char *buf) {
    const char *opts[4] = {"Under investigation", "Evidence review", "Evidence reviewed", "Closed"};
    int c;
    do {
        printf("Status: 1) Under investigation  2) Evidence review  3) Evidence reviewed  4) Closed\n");
        c = readInt("Choose status: ");
    } while (c < 1 || c > 4);
    strcpy(buf, opts[c - 1]);
}

int readInt(const char *prompt) {
    char line[64];
    int n;
    while (1) {
        printf("%s", prompt);
        if (!fgets(line, sizeof line, stdin)) return -1;
        if (sscanf(line, "%d", &n) == 1) return n;
        printf("  Please enter a valid number.\n");
    }
}

int equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}


Case *findCase(int id) {
    for (Case *p = head; p; p = p->next)
        if (p->id == id) return p;
    return NULL;
}

void appendCase(Case *node) {
    node->next = NULL;
    if (!head) { head = node; return; }
    Case *p = head;
    while (p->next) p = p->next;
    p->next = node;
}

void addCase(void) {
    int id = readInt("Case ID (positive number): ");
    if (id <= 0)         { printf("Invalid ID.\n"); return; }
    if (findCase(id))    { printf("Duplicate Case ID! Case %d already exists.\n", id); return; }

    Case *n = (Case *)malloc(sizeof(Case));
    if (!n) { printf("Memory allocation failed.\n"); return; }
    n->id = id;
    readLine("Crime type (Theft/Vandalism/Assault/Burglary): ", n->crimeType, STR_LEN);
    readLine("Location: ", n->location, STR_LEN);
    readDate("Date reported (DD-MM-YYYY): ", n->date);
    readLine("Evidence description: ", n->evidence, EVID_LEN);
    readLine("Officer in charge: ", n->officer, STR_LEN);
    strcpy(n->status, "Under investigation");
    appendCase(n);
    printf("Case %d added successfully.\n", id);
}

void printCase(const Case *c) {
    printf("%-6d %-12s %-18s %-12s %-16s %-20s\n",
           c->id, c->crimeType, c->location, c->date, c->officer, c->status);
}

void printHeader(void) {
    printf("\n%-6s %-12s %-18s %-12s %-16s %-20s\n",
           "ID", "Crime", "Location", "Date", "Officer", "Status");
    printf("--------------------------------------------------------------------------------------\n");
}

void displayCases(void) {
    if (!head) { printf("No cases recorded.\n"); return; }
    printHeader();
    int count = 0;
    for (Case *p = head; p; p = p->next) { printCase(p); count++; }
    printf("Total cases: %d\n", count);
}

void searchById(void) {
    int id = readInt("Enter Case ID to search: ");
    Case *c = findCase(id);
    if (!c) { printf("No case found with ID %d.\n", id); return; }
    printHeader();
    printCase(c);
    printf("Evidence: %s\n", c->evidence);
}

void searchByType(void) {
    char type[STR_LEN];
    readLine("Enter crime type to filter: ", type, STR_LEN);
    int found = 0;
    for (Case *p = head; p; p = p->next) {
        if (equalsIgnoreCase(p->crimeType, type)) {
            if (!found) printHeader();
            printCase(p);
            found++;
        }
    }
    if (!found) printf("No cases match '%s'.\n", type);
    else        printf("%d case(s) found.\n", found);
}

void updateCase(void) {
    int id = readInt("Enter Case ID to update: ");
    Case *c = findCase(id);
    if (!c) { printf("No case found with ID %d.\n", id); return; }

    int ch;
    do {
        printf("\n--- Updating case %d (ID cannot be changed) ---\n", c->id);
        printf("1. Crime type : %s\n", c->crimeType);
        printf("2. Location   : %s\n", c->location);
        printf("3. Date       : %s\n", c->date);
        printf("4. Evidence   : %s\n", c->evidence);
        printf("5. Officer    : %s\n", c->officer);
        printf("6. Status     : %s\n", c->status);
        printf("0. Done\n");
        ch = readInt("Which field to change? ");
        switch (ch) {
            case 1: readLine("New crime type: ", c->crimeType, STR_LEN); break;
            case 2: readLine("New location: ", c->location, STR_LEN); break;
            case 3: readDate("New date (DD-MM-YYYY): ", c->date); break;
            case 4: readLine("New evidence description: ", c->evidence, EVID_LEN); break;
            case 5: readLine("New officer in charge: ", c->officer, STR_LEN); break;
            case 6: readStatus(c->status); break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 0);
    printf("Case %d updated.\n", id);
}


void deleteCase(void) {
    int id = readInt("Enter Case ID to delete: ");
    Case *prev = NULL, *p = head;
    while (p && p->id != id) { prev = p; p = p->next; }
    if (!p) { printf("No case found with ID %d.\n", id); return; }

    printHeader();
    printCase(p);
    char ans[8];
    printf("Delete this case? (y/n): ");
    if (!fgets(ans, sizeof ans, stdin) || (ans[0] != 'y' && ans[0] != 'Y')) {
        printf("Deletion cancelled.\n");
        return;
    }
    if (!prev) head = p->next;              
    else       prev->next = p->next;
    free(p);
    printf("Case %d deleted.\n", id);
}


void saveCases(void) {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) { printf("Could not open %s for writing.\n", DATA_FILE); return; }
    for (Case *p = head; p; p = p->next)
        fprintf(f, "%d|%s|%s|%s|%s|%s|%s\n", p->id, p->crimeType, p->location,
                p->date, p->evidence, p->officer, p->status);
    fclose(f);
    printf("Cases saved to %s.\n", DATA_FILE);
}

int loadCases(void) {
    FILE *f = fopen(DATA_FILE, "r");
    if (!f) return 0;
    char line[512];
    int loaded = 0;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = '\0';
        Case *n = (Case *)malloc(sizeof(Case));
        if (!n) break;
        char *tok = strtok(line, "|");
        if (!tok) { free(n); continue; }
        n->id = atoi(tok);
        char *f1 = strtok(NULL, "|"), *f2 = strtok(NULL, "|"), *f3 = strtok(NULL, "|");
        char *f4 = strtok(NULL, "|"), *f5 = strtok(NULL, "|"), *f6 = strtok(NULL, "|");
        if (!f1 || !f2 || !f3 || !f4 || !f5 || !f6) { free(n); continue; }
        strncpy(n->crimeType, f1, STR_LEN - 1);  n->crimeType[STR_LEN - 1] = '\0';
        strncpy(n->location,  f2, STR_LEN - 1);  n->location[STR_LEN - 1]  = '\0';
        strncpy(n->date,      f3, STR_LEN - 1);  n->date[STR_LEN - 1]      = '\0';
        strncpy(n->evidence,  f4, EVID_LEN - 1); n->evidence[EVID_LEN - 1] = '\0';
        strncpy(n->officer,   f5, STR_LEN - 1);  n->officer[STR_LEN - 1]   = '\0';
        strncpy(n->status,    f6, STR_LEN - 1);  n->status[STR_LEN - 1]    = '\0';
        appendCase(n);
        loaded++;
    }
    fclose(f);
    return loaded;
}

void seedSampleData(void) {               /* same 4 fictional cases as the report */
    const char *d[4][7] = {
        {"101", "Theft",     "MG Road, Pune",     "15-08-2026", "CCTV footage, stolen bag",  "R. Sharma",  "Under investigation"},
        {"102", "Vandalism", "Sector 7, Nashik",  "18-08-2026", "Photos of damaged wall",    "P. Deshmukh","Under investigation"},
        {"103", "Assault",   "Camp Area, Pune",   "20-08-2026", "Witness statement notes",   "A. Patil",   "Evidence review"},
        {"104", "Burglary",  "College Road, Nashik","22-08-2026","Broken lock, fingerprints","S. Kulkarni","Under investigation"}
    };
    for (int i = 0; i < 4; i++) {
        Case *n = (Case *)malloc(sizeof(Case));
        if (!n) return;
        n->id = atoi(d[i][0]);
        strcpy(n->crimeType, d[i][1]);
        strcpy(n->location,  d[i][2]);
        strcpy(n->date,      d[i][3]);
        strcpy(n->evidence,  d[i][4]);
        strcpy(n->officer,   d[i][5]);
        strcpy(n->status,    d[i][6]);
        appendCase(n);
    }
}

void freeAll(void) {
    while (head) { Case *t = head; head = head->next; free(t); }
}


void menu(void) {
    printf("\n====== CRIME MANAGEMENT & EVIDENCE ORGANIZATION ======\n");
    printf(" 1. Add a case\n");
    printf(" 2. Display all cases\n");
    printf(" 3. Search case by ID\n");
    printf(" 4. Filter cases by crime type\n");
    printf(" 5. Update a case\n");
    printf(" 6. Delete a case\n");
    printf(" 0. Save and exit\n");
}

int main(void) {
    if (loadCases() == 0) seedSampleData();

    int choice;
    do {
        menu();
        choice = readInt("Enter choice: ");
        switch (choice) {
            case 1:  addCase();        break;
            case 2:  displayCases();   break;
            case 3:  searchById();     break;
            case 4:  searchByType();   break;
            case 5:  updateCase();     break;
            case 6:  deleteCase();     break;
            case 0:  saveCases();      break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    freeAll();
    return 0;
}