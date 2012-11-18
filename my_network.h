/**
 * send string
 * IN : descripteur de socket, pointeur sur la chaine
 * OUT: 1 si succes, 0 si echec
 *
 * fait : envoye la chaine a travers le socket, et s'assure que toute la chaine a ete envoyer
 **/

 int send_string(int sockfd, unsigned char *buffer){
	 int sentByte, byteToSend;
	 byteToSend = strlen(buffer);
	 while (byteToSend > 0){
		sentByte = send(sockfd, buffer, byteToSend, 0);
		if(sentByte == -1){
			return 0;
		}
		byteToSend -= sentByte;
		buffer += sentByte;
	 }
	 return 1;
 }


/**
 * receive line
 * IN : descripteur de socket, pointeur vers le tampon de destination
 * OUT: taille de la ligne sans les caracteres de fin de lignes
 *
 * fait : lit les données depuis la socket jusqu'a EOL 
 **/

 int recv_line(int sockfd, unsigned char *dest_buffer){
#define EOL "\r\n" //sequence EOL
#define EOL_SIZE 2
	unsigned char *ptr;
	int eol_matched = 0;

	ptr = dest_buffer;
	while(recv(sockfd, ptr, 1, 0) == 1){
		if(*ptr == EOL[eol_matched]){
			eol_matched ++;
			if(eol_matched == EOL_SIZE){
				*(ptr+1-EOL_SIZE) = '\0';
				return strlen(dest_buffer);
			}
		}
		else{
			eol_matched =0;
		}
		ptr++;
	}
	return 0;
}

/** 
 * constantes et structure pour l'heternet
 **/

#define ETHER_ADDR_LEN 6
#define ETHER_HDR_LEN 14

struct ether_hdr {
	unsigned char ether_dest_addr[ETHER_ADDR_LEN]; //adresse MAC de destination
	unsigned char ether_src_addr[ETHER_ADDR_LEN]; // adresse MAC de la source
	unsigned short ether_type; // type du paquet ethernet
};


/** 
 * constantes et structure pour IP
 **/

struct ip_hdr {
	unsigned char ip_version_and_header_length; // version et taille de l'entete
	unsigned char ip_tos; // type du service
	unsigned short ip_len; // longueur total
	unsigned short ip_id; // numero d'identification
	unsigned short ip_frag_offset; // decalage du fragment et indicateur
	unsigned char ip_ttl; // duree de vie
	unsigned char ip_type; // type de protocole
	unsigned short ip_checksum; //somme de controle
	struct in_addr ip_src_addr; // adresse IP source
	struct in_addr ip_dest_addr; // IP destinatio
};


/** 
 * constantes et structure pour TCP
 **/

 struct tcp_hdr {
	 unsigned short tcp_src_port;
	 unsigned short tcp_dest_port; // port source et destination de TCP
	 unsigned int tcp_seq; // numero de sequence tcp
	 unsigned int tcp_ack; //numero ack TCP
	 unsigned char reserved:4; // 4 bits parmi les 6 reserves
	 unsigned char tcp_offset:4; //decalage des données pour little indian
	 unsigned char tcp_flags; // indicateur reserve
#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PUSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
	 unsigned short tcp_window; // taille de la fenetre tcp
	 unsigned short tcp_checksum;
	 unsigned short tcp_urgent; //pointeur urgent tcp
 };


