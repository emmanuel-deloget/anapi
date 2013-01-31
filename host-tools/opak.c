#define _GNU_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct item_s item_t;
struct item_s {
	item_t *next;
	int type;
	char *id;
	char *path;
	unsigned int length;
	char *data;
};

struct option lopts[] = {
	"help", 0, NULL, 'h',		/* get help */
	"version", 0, NULL, 'V',	/* get version */
	"description", 1, NULL, 'd',	/* feed opak with a description of what to include */
	"output", 1, NULL, 'o',		/* give the path of the output file */
	"check" , 0, NULL, 'c', 	/* check if the configuration file is valid */
	NULL, 0, NULL, 0,
};

const char *sopts = "hvco:d:";

const char *opak_version = "0.1.0";

static char *strdup_nospc(char *id)
{
	unsigned int n = 0;
	char *oid = id;
	while (*oid && *oid != ' ' && *oid != '\t') {
		++n; 
		++oid;
	}
	if (!n)
		return strdup("");
	return strndup(id, n);
}

static unsigned int strlen_nospc(char *s)
{
	unsigned int n = 0;
	if (s) {
		while (*s && *s != ' ' && *s != '\t') {
			++n; 
			++s;
		}
	}
	return n;
}

static unsigned int item_count(item_t *first)
{
	unsigned int count = 0;
	while (first) {
		++count;
		first = first->next;
	}
	return count;
}

static item_t *item_back(item_t *first)
{
	item_t *prev = NULL;
	item_t *current = first;

	while (current) {
		prev = current;
		current = current->next;
	}
	return prev;
}

static item_t *item_push(item_t *first, int type, char *id, char *path)
{
	item_t *item = malloc(sizeof(item_t));

	item->type = type;
	item->id = strdup_nospc(id);
	item->path = strdup(path);
	item->next = NULL;
	item->data = NULL;
	item->length = 0;

	if (first) {
		item_t *back = item_back(first);
		back->next = item;
		return first;
	} else {
		return item;
	}
}

static void item_clear(item_t *first)
{
	item_t *current = first;

	while (current) {
		item_t *next = current->next;
		if (current->data)
			free(current->data);
		free(current->path);
		free(current);
		current = next;
	}
}

static int item_populate(item_t *item)
{
	int fd;
	struct stat statbuf;

	fd = open(item->path, O_RDONLY);
	if (fd < 0)
		return -1;
	if (fstat(fd, &statbuf) < 0)
		goto error;

	item->length = statbuf.st_size;
	item->data = malloc(item->length);
	if (!item->data)
		goto error;
	read(fd, item->data, item->length);
	return 0;

error:
	if (item->data)
		free(item->data);
	close(fd);
	return -1;
}

static void usage(char *p, int with_help)
{
	printf("%s: (dumbly) create an Object Package for use with anapi++/ekogen\n", p);
	printf("usage: %s options\n", p);
	printf(
		"options are: \n"
		"  -h|--help            : display the program help\n"
		"  -V|--version         : display the program version\n"
		"  -d|--description FILE: feed the program with a description file\n"
		"  -c|--check           : check the validity of the descirption file\n"
		"  -o|--output FILE     : setup the output file\n"
		);
	if (with_help) {
		printf(
			"\n"
			"DESCRIPTION FILE FORMAT\n"
			"The description file is line-based. Each empty line\n"
			"is discarded. Comment lines begin with # (the character\n"
			"MUST be the first character of the line). Other lines\n"
			"are description of the .opak file content.\n"
			"\n"
			"Each description is made of 3 fields: ID TYPE filepath\n"
			"  * ID is a 15 char max identifier with no space.\n"
                        "  * TYPE is one of RAW,IPNG,IJPEG,TXT.\n"
                        "  * filepath starts at the first non blank character after\n"
			"    TYPE and ends at the end of the line. All characters\n"
			"    between these two points is considered to be part of\n"
			"    the filename.\n"
			"    The filepath is etheir absolute or relative to the\n"
			"    current working directory.\n"
		);
	}
}

static void version(char *p)
{
	printf("%s: %s\n", p, opak_version);
}

static const char *skipw(const char* s)
{
	while (*s == ' ' || *s == '\t')
		++s;
	return s;
}

static const char *skipnw_w(const char* s)
{
	while (*s && *s != ' ' && *s != '\t')
		++s;
	while (*s == ' ' || *s == '\t')
		++s;
	return s;
}

static int parse_line(const char* line, char **id, char** type, char** path)
{
	const char* sid;
	const char* stype;
	const char* spath;

	sid = skipw(line);
	if (!*sid)
		return -1;
	stype = skipnw_w(sid);
	if (!*stype)
		return -1;
	spath = skipnw_w(stype);
	if (!*spath)
		return -1;

	*id = (char*)sid;
	*type = (char*)stype;
	*path = (char*)spath;
	return 0;
}

typedef enum {
	dit_none	= 0x00000000, 
	dit_raw		= 0x00000001,
	dit_text	= 0x00000002,
	dit_png		= 0x00010001,
	dit_jpeg	= 0x00010002,
} data_item_type_t;

static int type_from_string(const char *s)
{
	if (strncmp(s, "raw", 3) == 0)
		return dit_raw;
	else if (strncmp(s, "text", 4) == 0)
		return dit_text;
	else if (strncmp(s, "png", 3) == 0)
		return dit_png;
	else if (strncmp(s, "jpeg", 4) == 0)
		return dit_jpeg;
	else
		return dit_none;
}

static char *string_from_type(int t)
{
	switch (t) {
	case dit_raw: return "raw";
	case dit_text: return "text";
	case dit_png: return "png";
	case dit_jpeg: return "jpeg";
	default:
		return "none";
	}
}

item_t *get_all_items(const char *path)
{
	char *line;
	size_t length;
	ssize_t read;
	FILE *fdesc;
	item_t *items = NULL;
	int line_count = 0; 

	fdesc = fopen(path, "r");
	if (!fdesc)
		return NULL;

	length = 256;
	line = malloc(length);

	while ((read = getline(&line, &length, fdesc)) >= 0) {
		char *sid;
		char *stype;
		char *spath;
		int vtype;

		++line_count;
		line[read-1] = 0;
		if (line[0] == '#' || !line[0])
			continue;
		if (parse_line(line, &stype, &sid, &spath) < 0) {
			fprintf(stderr, "warning(%d): cannot parse line [%s], discard\n", line_count, line);
			continue;
		}
		if (strlen_nospc(sid) > 15) {
			fprintf(stderr, "warning(%d): object id [%s] is too long, discard\n", line_count, sid);
			continue;
		}
		vtype = type_from_string(stype);
		if (vtype == dit_none) {
			fprintf(stderr, "warning(%d): object type [%s] is unknown, discard\n", line_count, stype);
			continue;
		}
		items = item_push(items, vtype, sid, spath);
	}

	free(line);

	return items;
}

int populate_all_items(item_t *items)
{
	item_t *current = items;

	while (current) {
		if (item_populate(current) < 0) {
			fprintf(stderr, "warning; file [%s] cannot be read\n", current->path);
			return -1;
		}
		current = current->next;
	}
	return 0;
}

typedef struct dict_item_s dict_item_t;
struct dict_item_s {
	char id[16];
	uint32_t start;
	uint32_t length;
	uint32_t type;
	char reserved[12];
};

int write_dict_and_items(item_t *items, const char *output, int check)
{
	item_t *current;
	uint32_t icount = item_count(items);
	uint32_t pos = 0;

	if (!icount)
		return -1;

	int fd = open(output, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (fd < 0)
		return -1;

	pos += sizeof(icount);
	pos += icount * sizeof(dict_item_t);

	icount = htonl(icount);
	write(fd, &icount, sizeof(icount));

	current = items;
	printf("%16s %8s %9s %9s\n", "================", "========", "=========", "=========");
	printf("%-16s %-8s %-9s %-9s\n", "id", "type", "start", "length");
	printf("%16s %8s %9s %9s\n", "================", "========", "=========", "=========");
	while (current) {
		dict_item_t di;
		memset(&di, 0, sizeof(di));
		di.start = htonl(pos);
		di.length = htonl(current->length);
		di.type = htonl(current->type);
		sprintf(di.id, "%s", current->id);
		write(fd, &di, sizeof(di));
		if (check) {
			printf("%-16s %08x %-9u %-9u\n", di.id, ntohl(di.type), ntohl(di.start), ntohl(di.length));
		}
		pos += current->length;
		current = current->next;
	}
	printf("%16s %8s %9s %9s\n", "================", "========", "=========", "=========");
	current = items;
	while (current) {
		write(fd, current->data, current->length);
		current = current->next;
	}

	close(fd);
	return 0;
}

int main(int ac, char *av[])
{
	int opt;
	char *output = NULL;
	char *desc = NULL;
	int check = 0;
	item_t *items = NULL;
	int result;

	while ((opt = getopt_long(ac, av, sopts, lopts, NULL)) >= 0) {
		switch (opt) {
		case 'h':
			usage(av[0], 1);
			return EXIT_FAILURE;
			break;
		case 'V':
			version(av[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'o':
			output = strdup(optarg);
			break;
		case 'd':
			desc = strdup(optarg);
			break;
		case 'c':
			check = 1;
			break;
		default:
			fprintf(stderr, "error: unknown option %c\n", opt);
			usage(av[0], 0);
			return EXIT_FAILURE;
			break;
		}
	}

	if (!output || !desc) {
		fprintf(stderr, "error: options -d and -o are mandatory\n");
		usage(av[0], 0);
		return EXIT_FAILURE;
	}

	items = get_all_items(desc);
	if (!items) {
		fprintf(stderr, "error: no description in '%s'\n", desc);
		return EXIT_FAILURE;
	}

	result = populate_all_items(items);

	if (check) {
		item_t *current = items;
		printf("%-8s %-12s %s\n", "=======", "===========", "===========================================================================");
		printf("%-8s %-12s %s\n", "type", "file size", "file path");
		printf("%-8s %-12s %s\n", "=======", "===========", "===========================================================================");
		while (current) {
			printf("%-8s %-12lu [%s]\n", string_from_type(current->type),
					current->length, current->path);
			current = current->next;
		}
		printf("%-8s %-12s %s\n", "=======", "===========", "===========================================================================");
		printf("\n");
	}

	if (result == 0) {
		result = write_dict_and_items(items, output, check);
	}	

	item_clear(items);

	if (result < 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
