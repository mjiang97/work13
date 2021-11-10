#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

struct pop_entry {
  int year;
  int population;
  char boro[15];
};

long long file_size(char *r) {
    struct stat st;
    stat(r, &st);
    return st.st_size;
}

void read_csv(char *res) {
    int i;
    // Open file
    int file;
    file = open(res, O_RDONLY);
    if (file == -1){
        printf("Error:\n");
        printf("%s\n", strerror(errno));
        return;
    }
    // Read file
    int filesize = file_size(res);
    char data[filesize];
    int read_file;
    read_file = read(file, data, filesize);
    if (read_file == -1){
        printf("Error:\n");
        printf("%s\n", strerror(errno));
        return;
    }
    int num_lines = 0;
    for(i=0;i<filesize+1;i++) {
        if (data[i]=='\n') num_lines++;
    }
    int ind = 0, j;
    struct pop_entry entries[5*num_lines-5];
    char *boro[5] = {"Manhattan", "Brooklyn", "Queens", "Bronx", "Staten Island"};
    for(i=0;i<num_lines-1;i++){
        int line[6];
        while(data[ind]!='\n'){
            ind++;
        }
        ind++;
        sscanf(data+ind, "%d, %d, %d, %d, %d, %d\n", &line[0], &line[1], &line[2], &line[3], &line[4], &line[5]);
        for(j=1;j<6;j++){
            entries[5*i+j-1].year = line[0];
            strcpy(entries[(5*i)+j-1].boro, boro[j-1]);
            entries[(5*i)+j-1].population = line[j];
        }
    }
    int out = open("nyc_pop.data", O_CREAT | O_WRONLY, 0644);
    if(out==-1){
        printf("Error:\n");
        printf("%s\n", strerror(errno));
        return;
    }
    read_file = write(out, entries, sizeof(entries));
    if(out==-1){
        printf("Error:\n");
        printf("%s\n", strerror(errno));
        return;
    }
    printf("reading %s\n", res);
    printf("wrote %lld bytes to %s\n",file_size("nyc_pop.data"), res);
}

void read_data() {
    int filesize = file_size("nyc_pop.data");
    // Open file
    int file = open("nyc_pop.data", O_RDONLY);
    if(file==-1){
        printf("Error: \nData file doesn't exist, run -read_csv first.\n");
        return;
    }
    // Read file
    struct pop_entry *entries = malloc(file);
    int read_file = read(file, entries, filesize);
    if (read_file == -1){
        printf("Error:\n");
        printf("%s\n", strerror(errno));
        return;
    }
    // print 
    int i, num_entries = filesize / sizeof(struct pop_entry);
    for(i = 0; i < num_entries; i++){
        printf("%d:\t year: %d\t boro: %s\t pop: %d\n", i, entries[i].year, entries[i].boro, entries[i].population);
    }
    free(entries);
}

void add_data() {
    struct pop_entry new;
    char input[100];
    printf("Enter year boro pop:\n");
    fgets(input, sizeof(input), stdin);
    if(sscanf(input,"%d %1024[^0-9] %d", &new.year, new.boro, &new.population)!=3) {
        printf("Please enter it in correct format [int] [string] [int]\n");
        return;
    }
    int file = open("nyc_pop.data", O_WRONLY | O_APPEND);
    if(file==-1){
        printf("Error: \nData file doesn't exist, run -read_csv first.\n");
        return;
    }
    int err = write(file, &new, sizeof(new));
    if(err==-1){
        printf("Error:\n");
        return;
    }
}

void update_data() {
    struct pop_entry new;
    int num_entries = file_size("nyc_pop.data") / sizeof(struct pop_entry);
    char input[100];
    read_data();
    printf("Entry to update: ");
    int ind;
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%d", &ind);
    if(ind>=num_entries || ind < 0){
        printf("Not a possible entry\n");
        return;
    }
    int year, pop;
    char *boro;
    printf("Enter year boro pop:\n");
    fgets(input, sizeof(input), stdin);
    if(sscanf(input,"%d %1024[^0-9] %d", &new.year, new.boro, &new.population)!=3) {
        printf("Please enter it in correct format [int] [string] [int]\n");
        return;
    }
    int file = open("nyc_pop.data", O_RDWR);
    if(file==-1){
        printf("Error: \nData file doesn't exist, run -read_csv first.\n");
        return;
    }
    lseek(file, ind*sizeof(struct pop_entry), SEEK_SET);
    int err = write(file, &new, sizeof(new));
    if(err==-1){
        printf("Error:\n");
        return;
    }
    close(file);
}

int main(int argc, char *argv[]){
    int i;
    if (argv[1]==NULL) {
        printf("No commands given\n");
        return 0;
    }
    for (i = 1; i < argc; i++) {
        if (!(strcmp(argv[i],"-read_csv"))) {
            read_csv("nyc_pop.csv");
        } else if (!(strcmp(argv[i],"-read_data"))) {
            read_data();
        } else if (!(strcmp(argv[i],"-add_data"))) {
            add_data();
        } else if (!(strcmp(argv[i],"-update_data"))) {
            update_data();
        } else {
            printf("Invalid command:\n");}}
        return 0;
}