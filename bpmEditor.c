#include <curses.h>
#include <string.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 24

char image[24][80];

//curses initilize 
void initCurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1); 
}


//load for input bpm file
void loadPBM(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        endwin();
        exit(1);
    }

    char header[3];
    int width, height;


int fb = fgetc(file);
int sb = fgetc(file);
   
 if (fb != 80 || sb != 49) {
      printf("Unsupported file format\n");
      fprintf(stderr, "Unsupported file format\n");
      fclose(file);
       endwin();
       exit(1);
    }  
    
    
if (fgetc(file) == '\r') {
    fgetc(file);
} 


int c = fgetc(file);
while (c == '#'){
 while ((c!= 13) || (c!= 10)) {
     c = fgetc(file);
   } 
}

ungetc(c,file);

fscanf(file, " %d", &width);
fscanf(file, " %d", &height);
   
    if (width != WIDTH || height != HEIGHT) {
       
        fclose(file);
        endwin();
        printf("Invalid dimensions or does not match 80x24\n");
        exit(1);
    } 
    
   
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int pixel;
            if (fscanf(file, "%1d", &pixel) != 1) {
                fprintf(stderr, "Error reading image data\n");
                fclose(file);
                endwin();
                exit(1);
            }
            image[i][j] = (pixel == 1) ? '#' : ' ';
        }
    }

    fclose(file);
}

//same pbm file to output 
void savePBM(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to save file");
        endwin();
        exit(1);
    }

    fprintf(file, "P1\n");
    fprintf(file, "%d %d\n", WIDTH, HEIGHT);

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            fprintf(file, "%c ", image[i][j] == '#' ? '1' : '0');
        }
        fprintf(file, "\n");
    }

    fclose(file);
}


//draw the screen with the image/ Blank
void drawScreen() {
  clear();  
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
   mvaddch(i, j, image[i][j] == '#' ? '#' | A_REVERSE : ' ' | A_REVERSE);  
        }
    }
    move(0, 0);
    refresh(); 
}

//cursor control
void handleInput() {
    int x = 0, y = 0;  
    int ch;
    keypad(stdscr, TRUE); 
    curs_set(2); 

    while ((ch = getch()) != 27) { 
        switch (ch) {
            case KEY_LEFT:
                if (x > 0) x--;
                break;
            case KEY_RIGHT:
                if (x < WIDTH - 1) x++;
                break;
            case KEY_UP:
                if (y > 0) y--;
                break;
            case KEY_DOWN:
                if (y < HEIGHT - 1) y++;
                break;
            case ' ':  
         image[y][x] = (image[y][x] == '#') ? ' ' : '#';
  mvaddch(y, x, image[y][x] == '#' ? '#' | A_REVERSE : ' ' | A_REVERSE);
                break;
       }
     move(y, x);  
    refresh();  
    }
}



int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *output_file = NULL;

    
    for (int i = 1; i < argc; i++) {
    
    
        if (strncmp(argv[i], "in=", 3) == 0) {
            input_file = argv[i] + 3;            
             
        } 
        
        if (strncmp(argv[i], "out=", 4) == 0) {
            output_file = argv[i] + 4;
           
        }
    }

    if (input_file) {
        loadPBM(input_file);
    } else {
        memset(image, ' ', sizeof(image));
    }
    
    initCurses();
    drawScreen();
    handleInput();
   endwin();
   
     if (output_file) {
        savePBM(output_file);
     
     
     //output if no output file   
    } else {
    printf("P1\n");
    printf("80 24\n");
          for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 80; j++) {
            printf("%c",image[i][j] == '#'? '1' : '0');
             
            }
             printf("\n");
        }
        
        
    }
   
   
    return 0;
}



