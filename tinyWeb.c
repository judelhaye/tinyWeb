#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "error_and_dump.h"
#include "my_network.h"

#define PORT 1666 //connection web sur le port 1666
#define WEBROOT "./www" //repertoire racine du serveur

void handle_connection(int, struct sockaddr_in *);
int get_file_size(int);

int main(void){
	int sockfd, new_sockfd, yes =1;
	struct sockaddr_in host_addr, client_addr;
	socklen_t sin_size;

	printf("connection entrantes sur le port diabolique :  %d\n", PORT);
	//initialisation du socket
	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		fatal("in socket");
	}
	
	//mise en place de l'option REUSSEADDR
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		fatal ("mise en place del' option SO_REUSEADDR");
	}
	

	host_addr.sin_family = AF_INET; //ordre des octets de l'hote
	host_addr.sin_port = htons(PORT); //ordre des octets du reseau
	host_addr.sin_addr.s_addr = 0; //remplissage automatique de l'IP
	memset(&(host_addr.sin_zero), '\0', 8); // le reste de la structure a 90
	
	/**
	 * necessite des droit, du coup, soit execution par root (sudo)
	 * soit chown root puis chmod u+s sur l'executable.
	 **/
	if(bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1){
		fatal("binding socket");
	}

	if(listen(sockfd, 20) == -1){
		fatal("listening on socket");
	}

	while(42){
		sin_size = sizeof(struct sockaddr_in);
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);

		if(new_sockfd == -1){
			fatal("accepting connection");
		}

		handle_connection(new_sockfd, &client_addr);
	}
	return 0;
}

/**
 * handle connection
 * IN : descripteur pour la socket, pointeur sur structure sockaddr_in
 * OUT: /
 * 
 * fait : gere la connection a partir de l'adresse du client.
 **/

 void handle_connection(int sockfd, struct sockaddr_in *client_addr_ptr){
	 unsigned char *ptr, request[500], ressource[500];
	 int fd, length;

	 length = recv_line(sockfd, request);
	 printf("Got request form %s:%d \" %s\"\n", inet_ntoa(client_addr_ptr->sin_addr), ntohs(client_addr_ptr->sin_port), request);

	 ptr = strstr(request, " HTTP/"); // strstr : recherche dans uyne chaine.
	 if(ptr == NULL){
		 printf("connection non HTTP !\n");
	}
	else{
		*ptr = 0;
		ptr = NULL;
		if(strncmp(request, "GET ", 4) == 0){
			ptr = request+4; // une requete etant de la forme "GET /file.on.server" on jarcle les 4 premier caracteres pour 
					//  pour ne garder que le nom du fichier a balancer
		}
		if(strncmp(request, "HEAD ", 5) == 0){
			ptr = request+5; // meme principe, mais avec HEAD a la place de GET (donc un caractere en plus.
		}

		if(ptr == NULL){
			printf("\tUnknown request !\n");
		}
		else{
			if(ptr[strlen(ptr-1)] == '/'){
				/* strcat : concatenation de chaine de caractere en C, ultra pratique*/
				strcat(ptr, "index.html"); // si la requete finit par un "/", on renvoie au index.html associer.
				
			}

			strcpy(ressource, WEBROOT);
			strcat(ressource, ptr);

			fd = open(ressource, O_RDONLY, 0);

			if(fd == -1){
				printf("404 NOT FOUND\n");
				send_string(sockfd, "HTTP/1.0 404 NOT FOUND\r\n");
				send_string(sockfd, "serveur : tinyServeur\r\n\r\n");
				/**
				 * au lieu de balancer en chaine brut, on pourrait très bien 
				 * faire un 404.html. et envoyer le fichier.
				 * mais comme le principe c'est de pas avoir trouver de fichier
				 * je trouve pas ca super logique. 
				 **/
				send_string(sockfd, "<html><head><title>404</title></head>\r\n");
				send_string(sockfd, "<body><h1>URL Not Found !</h1></body></html>\r\n");
			}
			else{
				printf(" 200 OK \n");
				
				send_string(sockfd, "HTTP/1.0 200 OK\r\n");
				send_string(sockfd, "serveur : tinyServeur\r\n\r\n");
				if(ptr == request +4){
					if( (length = get_file_size(fd))== -1){
						fatal("getting ressource size");
					}
					ptr = (unsigned char *) ec_malloc(length);

					read(fd, ptr, length);
					send(sockfd, ptr, length, 0);
					free(ptr);
				}
				close(fd);
			}
		}
	}
	shutdown(sockfd, SHUT_RDWR);
 }

 int get_file_size(int fd){
	 struct stat stat_struct;

	 if(fstat(fd, &stat_struct) == -1){
		 return -1;
	 }

	 return (int) stat_struct.st_size;
 }


