#ifndef jaffa
#define jaffa

//gere les erreurs
void fatal (char* message){
	char fatal_error[100];
	
	strcpy(fatal_error, "[!!] Fatal Error ");
	strncat(fatal_error, message, 83);
	perror(fatal_error);
	exit(-1);
}

// malloc avvec gestion d'erreur
void* ec_malloc(unsigned int size){
	void *ptr;
	ptr = malloc(size);
	if(ptr == NULL){
		fatal("in ec_malloc() while allocate memory");
	}
	return ptr;
}

//afficher les octets en memoire sous forme hexa et char.
void dump (const unsigned char *data_buffer, const unsigned int length){
	unsigned char byte;

	unsigned i,j;
	for(i=0; i<length; i++){
		byte = data_buffer[i];
		printf("%02x ", data_buffer[i]);
		if(((i%16) == 15) || (i==length-1)){
			for(j=0; j<15-(i%16); j++){
				printf(" ");
			}
			printf("| ");
			for(j=(i-(i%16)); j<=i; j++){
				byte = data_buffer[j];
				if((byte >  31) && (byte <127)) {
					printf("%c", byte);
				}
				else{
					printf(".");
				}
			}
			printf("\n");
		}
	}
}


void dump_to_file (const unsigned char *data_buffer, const unsigned int length, FILE *fd){
	unsigned char byte;

	unsigned i,j;
	for(i=0; i<length; i++){
		byte = data_buffer[i];
		fprintf(fd, "%02x ", data_buffer[i]);
		if(((i%16) == 15) || (i==length-1)){
			for(j=0; j<15-(i%16); j++){
				fprintf(fd, " ");
			}
			fprintf(fd,"| ");
			for(j=(i-(i%16)); j<=i; j++){
				byte = data_buffer[j];
				if((byte >  31) && (byte <127)) {
					fprintf(fd,"%c", byte);
				}
				else{
					fprintf(fd, ".");
				}
			}
			fprintf(fd, "\n");
		}
	}
}

#endif
