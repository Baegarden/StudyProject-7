#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define MAX 256
#define SERVER_PORT 45000
#define PENDING 10

int DATA_SEND_RECV(int);
int UPLOAD(int);
int DOWNLOAD(int);
int LISTUSER(int);
int LISTFILE(int);
int EXIT(int);

int main(int argc, char * argv[])
{
	int select, sock_flag, conn_flag;
	struct sockaddr_in server_addr;

	if ((sock_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("socket ���� ���� \n");
		exit(0);
	}
	else
		printf("socket ���� ���� \n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(SERVER_PORT);

	if ((connect(sock_flag, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
	{
		printf("���� - Ŭ���̾�Ʈ ���� ���� \n");
		exit(0);
	}
	else
		printf("���� - Ŭ���̾�Ʈ ���� ���� \n");

	while (1)
	{

		printf("\n1.CHAT 2.UPLOAD 3.DOWNLOAD 4.LIST_USER 5.LIST_FILE 6.EXIT\n");
		printf("���� : ");
		scanf("%d", &select);
		printf("\n");

		switch (select) {
		case 1:
			DATA_SEND_RECV(sock_flag);
			break;
		case 2:
			UPLOAD(sock_flag);
			break;
		case 3:
			DOWNLOAD(sock_flag);
			break;
		case 4:
			LISTUSER(sock_flag);
			break;
		case 5:
			LISTFILE(sock_flag);
			break;
		case 6:
			EXIT(sock_flag);
			break;
		default:
			printf("�ٽ��Է��ϼ���\n");
		}
		if (select == 6)
			break;
	}
	close(sock_flag);
}

int DATA_SEND_RECV(int sock_flag)
{
	char msg[MAX], buf[MAX], outmsg[MAX];
	int select;
	int idx = 0;

	memset(msg, 0x00, sizeof(msg));
	strcpy(msg, "FLAG_CHAT|2016112618|");

	while (1)
	{
		printf("1.���� 2.�߽� 3.����\n");
		printf("�Է� : ");
		scanf("%d", &select);

		if (select == 1) // ����(�ѹ������� ����)
		{
			read(sock_flag, outmsg, sizeof(outmsg));
			printf("From others : %s\n", outmsg);
			printf("\n");
			memset(outmsg, 0x00, sizeof(outmsg));

		}
		else if (select == 2) // �߽�
		{
			printf("�޽����� �Է��ϼ��� : ");
			scanf("%s", buf);

			strcat(msg, buf);

			write(sock_flag, msg, sizeof(msg));

			read(sock_flag, outmsg, sizeof(outmsg));

			char *ptr = strtok(outmsg, "|");
			while (ptr != NULL)
			{
				idx++;
				ptr = strtok(NULL, "|");
				if (idx == 2)
				{
					printf("From server : %s\n", ptr);
					printf("\n");
				}
			}
			memset(msg, 0x00, sizeof(msg));
			memset(buf, 0x00, sizeof(buf));
		}
		else if (select == 3) // ����
			break;
		else
			printf("�ٽ��Է��ϼ���\n");
	}
}

int UPLOAD(int sock_flag)
{
	int pdf, n;
	char filename[MAX], msg[MAX], outmsg[200];
	printf("�����̸��� �Է��ϼ��� : ");
	scanf("%s", filename);

	pdf = open(filename, O_RDONLY);

	while ((n = read(pdf, outmsg, 200)) > 0) // ���� �о����
	{
		strcpy(msg, "FLAG_UPLOAD|2016112618|");
		strcat(msg, outmsg);
		write(sock_flag, msg, sizeof(msg));
		printf("%s\n", msg);
		memset(outmsg, 0x00, sizeof(outmsg));
		memset(msg, 0x00, sizeof(msg));
	}

	memset(outmsg, 0x00, 200);
	strcpy(outmsg, "FLAG_UPLOAD|2016112618|EOF"); // EOF ����
	write(sock_flag, outmsg, 200);

	memset(outmsg, 0x00, 200);
	read(sock_flag, outmsg, sizeof(outmsg)); // UPLOAD DONE Ȯ��
	printf("From server : %s\n", outmsg);
}

int DOWNLOAD(int sock_flag)
{
	int n, fd, idx;
	char outmsg[MAX];
	char msg[MAX] = "FLAG_DOWNLOAD|2016112618|";
	int f = 0;
	printf("�����̸��� �Է��ϼ��� : ");
	scanf("%s", outmsg);
	strcat(msg, outmsg);
	printf("�Է��� �����̸� : %s\n", msg);

	write(sock_flag, msg, sizeof(msg));
	memset(outmsg, 0x00, sizeof(msg));

	printf("From server : \n");

	fd = open("2016112618.pdf", O_WRONLY | O_CREAT | O_TRUNC, 0644);

	while ((n = read(sock_flag, outmsg, sizeof(outmsg))) > 0) // ���ϳ�������
	{
		idx = 0;
		printf("%s\n", outmsg);
		write(fd, outmsg, sizeof(outmsg));

		char *ptr = strtok(outmsg, "|");
		while (ptr != NULL)
		{
			idx++;
			ptr = strtok(NULL, "|");
			if (idx == 2)
			{
				if (strncmp("[DOWNLOAD]", ptr, 10) == 0)
					f = 1;
				if (strncmp("DOWNLOAD", ptr, 8) == 0)
					f = 1;
			}
		}
		memset(outmsg, 0x00, sizeof(outmsg));
		if (f == 1)
			break;
	}
}

int LISTUSER(int sock_flag)
{
	char buf[MAX] = "FLAG_LIST_USER|2016112618|NULL";
	char outmsg[MAX];
	int n;
	char *ptr;
	int m = 0, f = 0;

	write(sock_flag, buf, sizeof(buf));

	printf("From server\n");

	while ((n = read(sock_flag, outmsg, sizeof(outmsg))) > 0)
	{
		if (strlen(outmsg) < 225)
			f = 1;
		ptr = strtok(outmsg, " ");
		while (ptr != NULL)
		{
			if (m > 0 && strlen(ptr) == 11)
				printf("%s\n", ptr);
			ptr = strtok(NULL, " ");
			m++;
		}
		if (f == 1)
			break;
		memset(outmsg, 0x00, sizeof(outmsg));
	}
}

int LISTFILE(int sock_flag)
{
	char buf[MAX] = "FLAG_LIST_FILE|2016112618|NULL";
	char outmsg[MAX];
	int n;
	char *ptr;
	int m = 0;

	write(sock_flag, buf, sizeof(buf));

	printf("From server\n");

	while ((n = read(sock_flag, outmsg, sizeof(outmsg))) > 0)
	{
		ptr = strtok(outmsg, " ");

		while (ptr != NULL)
		{
			if (m > 0 && strlen(ptr) == 15)
				printf("%s\n", ptr);
			ptr = strtok(NULL, " ");
			m++;
		}
		if (n < 200)
			break;
		memset(outmsg, 0x00, sizeof(outmsg));
	}
}

int EXIT(int sock_flag)
{
	char msg[MAX] = "FLAG_EXIT|2016112618|EXIT";
	char outmsg[MAX];
	int idx = 0;

	write(sock_flag, msg, sizeof(msg));

	read(sock_flag, outmsg, sizeof(outmsg));

	char *ptr = strtok(outmsg, "|");
	while (ptr != NULL)
	{
		idx++;
		ptr = strtok(NULL, "|");
		if (idx == 2)
		{
			printf("From server : %s\n", ptr);
		}
	}
}
