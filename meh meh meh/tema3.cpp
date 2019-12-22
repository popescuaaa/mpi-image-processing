#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iterator>
#include <mpi.h>
using namespace std;

#define EXIT -1
#define WIDTH_TAG 1
#define ACK_TAG 2
#define SOBEL_TAG 4
#define MEAN_REMOVAL_TAG 5
#define MATRIX_PROCESSED_TAG 6
#define SIZE_TAG 7
#define END_TAG 8
#define STATISTIC_TAG 9
#define STATISTIC_SIZE_TAG 10

/*
@param name - numele iamgini din care se citeste matricea de pixeli
@param first_line - prima linie din fisier
@param second_line - a doua linie din fisier
@param width - latimea matricei
@param height - inaltimea matricei
@param e_255 - valoarea maxima a pixelului
@param matrix - matricea de pixeli stocata ca un vector de linii

Citeste matricea de pixeli si metadatele dintr-o imagine pe
care le retine in campurile primite ca parametru
Pentru citire am folosit input file stream (ifstream in c++) din care citesc primele
4 linii, iar apoi citesc fiecare pixel, indiferent de separatorii dintre pixeli
*/
void read_image(const char *name, string *first_line, string *second_line, int *width, int *height, int *e_255, vector< vector<int> > *matrix) {
    ifstream ifs(name);
    string s;
    int val, i, j;
    vector<int> line;

    getline(ifs, *first_line);
    getline(ifs, *second_line);

    ifs >> (*width) >> (*height) >> (*e_255);

    for (i = 0; i < (*width) + 2; i++) {
        line.push_back(0);
    }

    (*matrix).push_back(line);

    for (i = 0; i < *height; i++) {
        line.clear();
        line.push_back(0);

        for (j = 0; j < *width; j++) {
            ifs >> val;
            line.push_back(val);
        }

        line.push_back(0);
        (*matrix).push_back(line);
    }

    line.clear();
    for (i = 0; i < (*width) + 2; i++) {
        line.push_back(0);
    }

    (*matrix).push_back(line);
    ifs.close();
}

/*
@param line - un string pe care il imparte in token-uri dupa caracterul spatiu

Realizeaza impartirea string-ului primit ca parametru in token-uri, stocate
ca un vector de string-uri, dupa caracterul spatiu.
*/
vector<string> split(string line) {
    vector<string> tokens;
    istringstream iss(line);
        
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(tokens));

    return tokens;
}

/*
@param name - numele iamgini in care se scrie matricea de pixeli
@param first_line - prima linie din fisier
@param second_line - a doua linie din fisier
@param width - latimea matricei
@param height - inaltimea matricei
@param e_255 - valoarea maxima a pixelului
@param matrix - matricea de pixeli stocata ca un vector de linii

Scrie matricea de pixeli si metadatele intr-un fisier imagine
plecand de la campurile primite ca parametru
Spre deosebire de citire, in care foloseam stream-uri, pentru
scriere nu mai folosesc stream-uri, deoarece este prea lenta execuia
*/
void write_image(const char *name, string *first_line, string *second_line, int *width, int *height, int *e_255, vector< vector<int> > *matrix) {
    FILE *output = fopen(name, "w");
    int i, j;

    fprintf(output, "%s\n", first_line->c_str());
    fprintf(output, "%s\n", second_line->c_str());
    fprintf(output, "%d %d\n", *width, *height);
    fprintf(output, "%d\n", *e_255);

    for (i = 1; i <= (*height); i++) {
        for (j = 1; j <= (*width); j++) {
            fprintf(output, "%d\n", (*matrix)[i][j]);
        }
    }
    
    fclose(output);
}

/*
@param name - numele fiserului ce contine topologia
@param nProcesses - numarul de procese care trebuie sa isi afle topologia
@param pid - id-ul procesului curent care citeste din fisier

Retruneaza lista de vecini a proceslului ce are id-ul pid.
Pentru acest lucru citesc linie cu linie, o impart dupa spatiu,
verific daca primul token contine id-ul procesului curent, iar daca se
intampla acest lucru citesc toti vecinii dupa care opresc executia,
deoarece fiecare proces trebuie sa returneze doar vecinii sai
*/
vector<int> get_neighbours(const char *name, int nProcesses, int pid) {
    int i, j;
    ifstream ifs(name, ifstream::in);
    string line;
    vector<int> neighbours;

    for (i = 0; i < nProcesses; i++) {
        int int_pid;
        vector<string> tokens;
        string string_pid;

        getline(ifs, line);
        tokens = split(line);

        string_pid = tokens[0].substr(0, tokens[0].length() - 1);
        int_pid = atoi(string_pid.c_str());
        
        if (int_pid == pid) {
            for (j = 1; j < tokens.size(); j++) {
                neighbours.push_back(atoi(tokens[j].c_str()));
            }

            break;
        }
    }

    ifs.close();
    return neighbours;
}

/*
@param matrix - matricea de pixeli asupra careia se aplica filtrul
@param start - linia de inceput pentru care se aplica filtrul
@param finish - linia de final pentru care se aplica filtrul
@param filter_tag - parametru care specifica ce filtru trebuie sa seaplice

Realizeaza aplicarea unui filtru pe matricea primita ca parametru
pe liniile din intrevalul [start; finish).
Se parcurge matricea si la fiecare pas se verifica tag-ul de filtru ce trebuie aplicat
*/
vector< vector<int> > apply_filter(vector< vector<int> > matrix, int start, int finish, int filter_tag) {
    int i, j, width;
    vector< vector<int> > new_matrix;

    new_matrix.resize(matrix.size());
    width = matrix[start].size() - 2;

    for (i = 0; i < matrix.size(); i++) {
        new_matrix[i].resize(matrix[start].size(), 0);
    }

    for (i = start; i < finish; i++) {
        for (j = 1; j <= width; j++) {
            if (filter_tag == MEAN_REMOVAL_TAG) {
                new_matrix[i][j] = (9 * matrix[i][j] -
                        matrix[i - 1][j - 1] - matrix[i - 1][j] - matrix[i - 1][j + 1] -
                        matrix[i][j - 1] - matrix[i][j + 1] - 
                        matrix[i + 1][j - 1] - matrix[i + 1][j] - matrix[i + 1][j + 1]); 
            } else if (filter_tag == SOBEL_TAG) {
                new_matrix[i][j] = ((matrix[i - 1][j - 1] - matrix[i - 1][j + 1]) + 
                        2 * (matrix[i][j - 1] - matrix[i][j + 1]) +
                        (matrix[i + 1][j - 1] - matrix[i + 1][j + 1]));
                new_matrix[i][j] += 127;
            }
            
            if (new_matrix[i][j] < 0) {
                new_matrix[i][j] = 0;
            } 

            if (new_matrix[i][j] > 255) {
                new_matrix[i][j] = 255;
            }
        }
    }

    return new_matrix;
}

/*
@param matrix - matricea ce se imaprte in blocuri pentr a fi trimisa destinatiei
@param start - indicele de start al portiunii din submatricea ce se va trimite destinatiei
@param finish - indicele de final al portiunii din submatricea ce se va trimite destinatiei
@param filter_tag - tag-ul de filtru, reprezinta tag-ul pe care se trimite mesajul

Trimite din matricea primita ca parametru doar liniile din intervalul [start; finish), iar
aceste bucati sunt trimise cu tag-ul filtrului, dar inainte de acest lucru trimite nodului copil
pe tagul de dimensiune, SIZE_TAG, numarul de linii pe care acel copil va trebui sa le primeasca.
Un nod intermediar va astepta dimensiunea pe tag-ul de dimensiune, iar matricea o va astepta
pe MPI_ANY_TAG, valoarea acelui tag diind importanta doar la nodurile frunza pentru aplicarea filtrului
*/
void send_submatrix(vector< vector<int> > matrix, int start, int finish, int destination, int filter_tag) {
    int dim = finish - start;

    MPI_Send(&dim, 1, MPI_INT, destination, SIZE_TAG, MPI_COMM_WORLD);

    for (int i = start; i < finish; i++) {
        MPI_Send(&(matrix[i][0]), matrix[i].size(), MPI_INT, destination, filter_tag, MPI_COMM_WORLD);
    }
}

/*
@param matrix - matricea ce se imaprte in blocuri pentr a fi trimisa destinatiei
@param start - indicele de start al portiunii din submatricea ce se va trimite destinatiei
@param finish - indicele de final al portiunii din submatricea ce se va trimite destinatiei
@param filter_tag - tag-ul de filtru, reprezinta tag-ul pe care se trimite mesajul

Trimite matricea prelucrata inapoi la parinte, dar pe tag-ul care indica ca matricea a fost procesata,
MATRIX_PROCESSED_TAG
De aceasta data nu se mai trimite dimensiunea, deoarece parintele stie cat a trimis la copil si va sti
cat sa astepte inapoi
*/
void send_submatrix_to_parent(vector< vector<int> > matrix, int start, int finish, int destination) {
    int dim = finish - start;

    for (int i = start; i < finish; i++) {
        MPI_Send(&(matrix[i][0]), matrix[i].size(), MPI_INT, destination, MATRIX_PROCESSED_TAG, MPI_COMM_WORLD);
    }
}

/*
@param height - inaltimea submatricei ce se va primi
@param width - latimea matricei
@param from - id-ul procesului de la care se primeste matricea
@param status - structura in care se retin mai multe detalii din MPI

Primeste submatricea de la o sursa, memorand tag-ul in structura status, aceasta
avand importanta in frunze cand trebuie sa se cunoasca tag-ul
*/
vector< vector<int> > recv_submatrix(int height, int width, int from, MPI_Status *status) {
    int i;
    vector< vector<int> > matrix;

    matrix.resize(height);

    for (i = 0; i < height; i++) {
        matrix[i].resize(width, 0);
    }

    for (i = 0; i < height; i++) {
        MPI_Recv(&(matrix[i][0]), width, MPI_INT, from, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    }

    return matrix;
}

/*
@param filter - filtrul citit din fisier

Returneaza idnicatorul de filtru, ca numar, pornind de la
string-ul ce se citeste din fisierul de comenzi
*/
int get_filter_tag(string filter) {
    if (filter.compare("sobel") == 0) {
        return SOBEL_TAG;
    } else if (filter.compare("mean_removal") == 0) {
        return MEAN_REMOVAL_TAG;
    }

    return -1;
}

/*
@param file_name - numele fisierului in care se scrie statistica
@param statistics - statisitca retinuta sub forma unei mapari de la int la in
@param nProcesses - numarul de procese ce sunt continute in statistica

Scrie in fisier statistica, aceasta avand urmatoare semnificate
(statistics[i] = numarul de linii procesate de procesul "i")
*/
void write_statistics(const char *file_name, map<int, int> statistics, int nProcesses) {
	int i;
	FILE *file = fopen(file_name, "w");

	for (i = 0; i < nProcesses; i++) {
		fprintf(file, "%d: %d\n", i, statistics[i]);
	}

	fclose(file);
}

int main(int argc, char *argv[]) {
    int i, j, rank, nProcesses, start, finish, part_size = 0, ack = -1, parent = -1, send_size;
    int line, col, filter_tag, end, processed_lines = 0;
    vector<int> neighbours, children;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

    MPI_Status status;

    /* fiecare proces trebuie sa isi afle vecinii */
    neighbours = get_neighbours(argv[1], nProcesses, rank);

    if (rank == 0) {
        int width, height, e_255, ack, current_line, commands, command_number;
        map<int, int> statistics;
        ifstream ifs(argv[2], ifstream::in);

        ifs >> commands;

        /* se parcurg comenzile din fisier pentru a se executa */
        for (command_number = 0; command_number < commands; command_number++) { 
            string first_line, second_line;
            vector< vector<int> > matrix, new_matrix, child_matrix;
            string filter_as_string, image_to_open, image_out_name;

            ifs >> filter_as_string >> image_to_open >> image_out_name;

            filter_tag = get_filter_tag(filter_as_string);
            read_image(image_to_open.c_str(), &first_line, &second_line, &width, &height, &e_255, &matrix);

            /* dupa ce a citit matricea radacina trimite latimea acesteia, e folosit si pentru a descoperi parintii */
            for (i = 0; i < neighbours.size(); i++) {
                MPI_Send(&width, 1, MPI_INT, neighbours[i], WIDTH_TAG, MPI_COMM_WORLD);
            }

            /* se astepta un ack, pentru a fi siguri ca toate procesele au primit latimea */
            for (i = 0; i < neighbours.size(); i++) {
                MPI_Recv(&ack, 1, MPI_INT, neighbours[i], ACK_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            /* se calculeaza nr de linii de procesat pt fiecare vecini al radacinii */
            send_size = (matrix.size() - 2) / neighbours.size();

            start = 0;
            finish = start + send_size + 2;

            /* la toti vecinii exceptand ultimul se trimit blocuri egale */
            for (i = 0; i < neighbours.size() - 1; i++) {
                send_submatrix(matrix, start, finish, neighbours[i], filter_tag);
                start += send_size;
                finish += send_size;
            }

            /* la utimul vecin se trimite restul */
            send_submatrix(matrix, start, matrix.size(), neighbours[neighbours.size() - 1], filter_tag);
        
            new_matrix.resize(matrix.size());
            for (i = 0; i < matrix.size(); i++) {
                new_matrix[i].resize(matrix[0].size());
            }

            /* se creeaza noua matrice */
            current_line = 0;
            for (i = 0; i < matrix[0].size(); i++) {
                new_matrix[current_line][i] = matrix[0][i];
                new_matrix[matrix.size() - 1][i] = matrix[matrix.size() - 1][i];
            }

            /* se asteapta matricele de la copii se se preia din acestea doar miezul, fara prima si ultima linie */
            for (i = 0; i < neighbours.size() - 1; i++) {
                child_matrix = recv_submatrix(send_size + 2, matrix[0].size(), neighbours[i], &status);

                for (line = 1; line < child_matrix.size() - 1; line++) {
                    current_line++;
                    for (col = 0; col < child_matrix[0].size(); col++) {
                        new_matrix[current_line][col] = child_matrix[line][col];
                    }
                }
            }

            /* se preia si de la ultimul vecin matricea si se introduce miezul acesteia in matricea rezultata */
            child_matrix = recv_submatrix(matrix.size() - start, matrix[0].size(), neighbours[neighbours.size() - 1], &status);

            for (line = 1; line < child_matrix.size() - 1; line++) {
                current_line++;

                for (col = 0; col < child_matrix[0].size(); col++) {
                    new_matrix[current_line][col] = child_matrix[line][col];
                }
            }

            /* se scrie imaginea in fisier */
            write_image(image_out_name.c_str(), &first_line, &second_line, &width, &height, &e_255, &new_matrix);
        }

        width = EXIT;

        /* radacina trimite la toti vecinii un mesajul pe tag-ul de terminare */
        for (i = 0; i < neighbours.size(); i++) {
            MPI_Send(&width, 1, MPI_INT, neighbours[i], END_TAG, MPI_COMM_WORLD);
        }

        /* se initializeaza statistica */
        for (i = 0; i < nProcesses; i++) {
        	statistics[i] = 0;
        }

        /* pentru fiecare vecin radacina trebuie sa primeasca un vector ce contine statistica,
        iar la final se asambleaza aceasta statistica */
        for (i = 0; i < neighbours.size(); i++) {
        	vector<int> statistics_received;
        	int statistic_dim;

        	MPI_Recv(&statistic_dim, 1, MPI_INT, neighbours[i], STATISTIC_SIZE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        	statistics_received.resize(statistic_dim);
        	MPI_Recv(&statistics_received[0], statistic_dim, MPI_INT, neighbours[i], STATISTIC_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        	for (j = 0; j < statistics_received.size() - 1; j = j + 2) {
        		statistics[statistics_received[j]] = statistics_received[j + 1];
        	}
        }

        /* se scrie statistica in fisier si se incheie executia procesului */
        write_statistics(argv[3], statistics, nProcesses);
    } else {
        while (1) {
            int recv_width, worked_lines, recv_size, star, finish, current_line;
            int line, col;
            vector< vector<int> > matrix_part, new_matrix, child_matrix;
        
        	/* initial fiecare nod intermediar, daca nu isi cunoaste parintele asteapta de la oricine, altfel asteapta de al parinte
        	latimea ueni linii din matricea de pixeli. Acest mesaj e folosit si pentru a se decoperi parintii fiecarui nod */
            if (parent == -1) {
                MPI_Recv(&recv_width, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            } else {
                MPI_Recv(&recv_width, 1, MPI_INT, parent, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }
            
            /* Daca s-a primit un mesaj cu tag de terminare se transmite mai jos, se astepta statistica,
            se asambleaza, se transmite parintelui si se incheie executia */
            if (status.MPI_TAG == END_TAG) {
            	vector<int> ranks_and_lines;
            	int statistic_dim;

            	/* Daca e frunza atunci se transmite parintelui doar rank-ul si numarul de linii procesate */
                if (children.size() == 0) {
                	ranks_and_lines.push_back(rank);
                	ranks_and_lines.push_back(processed_lines);

                	statistic_dim = ranks_and_lines.size();

                	MPI_Send(&statistic_dim, 1, MPI_INT, parent, STATISTIC_SIZE_TAG, MPI_COMM_WORLD);
                	MPI_Send(&ranks_and_lines[0], 2, MPI_INT, parent, STATISTIC_TAG, MPI_COMM_WORLD);
                } else {
                	/* Se transmite copiilor mesajul pe tag-ul de terminare */
                	for (i = 0; i < children.size(); i++) {
                    	MPI_Send(&recv_width, 1, MPI_INT, children[i], END_TAG, MPI_COMM_WORLD);
                	}

                	/* De l feicare copil se asteapta statistica si se compune statistica ce se va transmite parintelui*/
                	for (i = 0; i < children.size(); i++) {
                		int child_statistic_size;
                		vector<int> child_statistic;

                		MPI_Recv(&child_statistic_size, 1, MPI_INT, children[i], STATISTIC_SIZE_TAG, MPI_COMM_WORLD, &status);
                		child_statistic.resize(child_statistic_size);
                		MPI_Recv(&child_statistic[0], child_statistic_size, MPI_INT, children[i], STATISTIC_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                		for (j = 0; j < child_statistic.size(); j++) {
                			ranks_and_lines.push_back(child_statistic[j]);
                		}
                	}

                	/* Se transmite dimensiunea statisticii parintelui, pentru a-l anunta cat are de primit, iar apoi statistica in sine */
                	statistic_dim = ranks_and_lines.size();
                	MPI_Send(&statistic_dim, 1, MPI_INT, parent, STATISTIC_SIZE_TAG, MPI_COMM_WORLD);
                	MPI_Send(&ranks_and_lines[0], statistic_dim, MPI_INT, parent, STATISTIC_TAG, MPI_COMM_WORLD);
                }

                break;
            }

            /* Daca procesul nu are parinte atunci se afla parintele, id-ul procesului de la care
            a primit primul mesaj si se construieste lista de copii */
            if (parent == -1) {
                parent = status.MPI_SOURCE;

                for (i = 0; i < neighbours.size(); i++) {
                    if (neighbours[i] != parent) {
                        children.push_back(neighbours[i]);
                    }
                }
            }

            /* Se transmite feicarui copil latimea unei linii din matricea de pixeli */
            for (i = 0; i < children.size(); i++) {
                MPI_Send(&recv_width, 1, MPI_INT, children[i], WIDTH_TAG, MPI_COMM_WORLD);
            }

            /* Se asteapta confirmarea de la fiecare copil, pentru a fi sigur ca toti stiu latimea unei linii */
            for (i = 0; i < children.size(); i++) {
                MPI_Recv(&ack, 1, MPI_INT, children[i], ACK_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            /* Se trimite confirmarea parintelui */
            MPI_Send(&ack, 1, MPI_INT, parent, ACK_TAG, MPI_COMM_WORLD);

            /* Se asteapta primirea diemnsiunii (in numar de linii) a submatricei ce va fi trimisa de parinte */
            MPI_Recv(&recv_size, 1, MPI_INT, parent, SIZE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        	/* Daca dimensiunea e mai mica decat 2, atunci nu am ce procesa, trimit copiilor pentr a-i anunta si pe ei */
            if (recv_size <= 2) {
                for (i = 0; i < children.size(); i++) {
                    MPI_Send(&recv_size, 1, MPI_INT, children[i], SIZE_TAG, MPI_COMM_WORLD);
                }
            } else {
            	/* Altfel primesc matricea, iar altimea este + 2, incluzan zerourile de al inceput si final de linie */
                matrix_part = recv_submatrix(recv_size, recv_width + 2, parent, &status);
            
            	/* Daca sunt copii, impart matricea, o trimit, si apoi astept reasamblarea ei */
                if (children.size() > 0) {
                    send_size = (matrix_part.size() - 2) / children.size();

                    /* Daca am mai multe linii decat copii trimit latoti copii, altfel doar la primul copil */
                    if (send_size > 0) {
                        start = 0;
                        finish = start + send_size + 2;

                        /* Se imaprte matricea si se trimite primilor copii in bucati egale */
                        for (i = 0; i < children.size() - 1; i++) {
                            send_submatrix(matrix_part, start, finish, children[i], status.MPI_TAG);
                            start += send_size;
                            finish += send_size;
                        }

                        /* Ultimul copil priemste restul */
                        send_submatrix(matrix_part, start, matrix_part.size(), children[children.size() - 1], status.MPI_TAG);

                        /* Se contruieste noua matrice */
                        new_matrix.resize(matrix_part.size());
                        for (i = 0; i < matrix_part.size(); i++) {
                            new_matrix[i].resize(matrix_part[0].size());
                        }

                        current_line = 0;
                        for (i = 0; i < matrix_part[0].size(); i++) {
                            new_matrix[current_line][i] = matrix_part[0][i];
                            new_matrix[matrix_part.size() - 1][i] = matrix_part[matrix_part.size() - 1][i];
                        }

                        /* De la fiecare copil preiau doar miezul, bucata pe care s-a aplicat filtrul */
                        for (i = 0; i < children.size() - 1; i++) {
                            child_matrix = recv_submatrix(send_size + 2, recv_width + 2, children[i], &status);

                            for (line = 1; line < child_matrix.size() - 1; line++) {
                                current_line++;
                                for (col = 0; col < child_matrix[0].size(); col++) {
                                    new_matrix[current_line][col] = child_matrix[line][col];
                                }
                            }
                        }

                        /* Preiau si de la ultimul copil restul de matrice*/
                        child_matrix = recv_submatrix(matrix_part.size() - start, recv_width + 2, children[children.size() - 1], &status);

                        for (line = 1; line < child_matrix.size() - 1; line++) {
                            current_line++;
                            for (col = 0; col < child_matrix[0].size(); col++) {
                                new_matrix[current_line][col] = child_matrix[line][col];
                            }
                        }
                    } else {
                    	/* Am mai putine linii decat copii, deci trimit doar primului copil */
                        send_submatrix(matrix_part, 0, matrix_part.size(), children[0], status.MPI_TAG);

                        /* Celorlalti copii le trimit un mesaj cu dimensiunea asteptata 0, pentru a-i anunta ca nu
                        au nimic de procesat si sa nu astepte inutil*/
                        for (i = 1; i < children.size(); i++) {
                        	int dim = 0;
                        	MPI_Send(&dim, 1, MPI_INT, children[i], SIZE_TAG, MPI_COMM_WORLD);
                        }

                        /*Asamble matricea primita de la primul copil */
                        current_line = 0;
                        for (i = 0; i < matrix_part[0].size(); i++) {
                            new_matrix[current_line][i] = matrix_part[0][i];
                        }

                        child_matrix = recv_submatrix(matrix_part.size(), recv_width + 2, children[0], &status);

                        for (line = 1; line < child_matrix.size(); line++) {
                            current_line++;
                            for (col = 0; col < child_matrix[0].size(); col++) {
                                new_matrix[current_line][col] = child_matrix[line][col];
                            }
                        }
                    }

                    /* Trimit matricea parintelui */
                    send_submatrix_to_parent(new_matrix, 0, new_matrix.size(), parent);
                } else {
                	/* O frunza a primit bucata pe care trebuie sa o proceseze si aplica filtrul in functie de tagul
                	pe care a primit matricea, luat din structurea status */
                    new_matrix = apply_filter(matrix_part, 1, matrix_part.size() - 1, status.MPI_TAG);
                    processed_lines += (matrix_part.size() - 2);

                    /* Se trimite matricea parintelui */
                    send_submatrix_to_parent(new_matrix, 0, new_matrix.size(), parent); 
                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}
