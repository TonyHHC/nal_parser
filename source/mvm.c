/* make -f mvm_adt.mk */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mvm.h"

#define _PRINT_MVM_CONTENT_MAX_LENGTH 10000

mvm* mvm_init(void){
	mvm *p = (mvm*)malloc(sizeof(mvm));

	p->head = NULL;
	p->numkeys = 0;

	return p;
}

int mvm_size(mvm* m){
	if (m == NULL)
		return 0;

	return m->numkeys;
}

void mvm_insert(mvm* m, char* key, char* data){
	mvmcell *old_head;
	mvmcell *np;
	char *new_key, *new_data;

	if (m == NULL || key == NULL || data == NULL)
		return;

	np = (mvmcell*)malloc(sizeof(mvmcell));

	new_key = (char*)malloc(strlen(key) + 1);
	strcpy(new_key, key);

	new_data = (char*)malloc(strlen(data) + 1);
	strcpy(new_data, data);

	np->key = new_key;
	np->data = new_data;
	np->next = NULL;

	if (m->head == NULL)	{
		m->head = np;
	}
	else	{
		old_head = m->head;
		m->head = np;
		np->next = old_head;
	}

	m->numkeys++;
}

char* mvm_print(mvm* m){
	mvmcell *p;

	char *strResult;
	char *strTmp;

	if (m == NULL)
		return NULL;

	strResult = (char*)malloc(_PRINT_MVM_CONTENT_MAX_LENGTH * sizeof(char));
	strTmp = (char*)malloc(_PRINT_MVM_CONTENT_MAX_LENGTH * sizeof(char));
	sprintf(strResult, "");

	p = m->head;

	while (p != NULL)	{
		if (p->key != NULL && p->data != NULL){
			sprintf(strTmp, "[%s](%s) ", p->key, p->data);
			strcat(strResult, strTmp);
		}
		p = p->next;
	}

	return strResult;
}

void mvm_print_2_console(mvm* m){
	mvmcell *p;

	if (m == NULL)
		return;

	p = m->head;

	while (p != NULL){
		if (p->key != NULL && p->data != NULL)
			printf("[%s](%s)\n", p->key, p->data);
		p = p->next;
	}
}

char* mvm_update(mvm* m, char* key, char* newdata){
	mvmcell *p;
	char *data;

	p = m->head;

	while (p != NULL){
		if (strcmp(p->key, key) == 0){
			free(p->data);

			data = (char*)malloc(strlen(newdata) + 1);
			strcpy(data, newdata);
			p->data = data;

			return p->data;
		}

		p = p->next;
	}

	return NULL;
}

void mvm_update_ext(mvm* m, char* key, char* newdata){
	mvmcell *p;
	char *data;

	p = m->head;

	while (p != NULL){
		if (strcmp(p->key, key) == 0){
			free(p->data);

			data = (char*)malloc(strlen(newdata) + 1);
			strcpy(data, newdata);
			p->data = data;
		}

		p = p->next;
	}

	mvm_insert(m, key, newdata);
}

void mvm_delete(mvm* m, char* key){
	mvmcell *temp, *prev;
	prev = NULL;

	if (m == NULL || key == NULL)
		return;

	temp = m->head;

	/* if head*/
	if (temp != NULL && strcmp(temp->key, key) == 0){
		m->head = temp->next;
		free(temp->key);
		free(temp->data);
		free(temp);
		m->numkeys--;
		return;
	}

	/* if not head */
	while (temp != NULL && strcmp(temp->key, key) != 0){
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL) return;

	prev->next = temp->next;
	m->numkeys--;

	free(temp->key);
	free(temp->data);
	free(temp);

}

char* mvm_search(mvm* m, char* key){
	mvmcell *p;
	p = m->head;

	while (p != NULL){
		if (strcmp(p->key, key) == 0)
			return p->data;

		p = p->next;
	}

	return NULL;
}

char** mvm_multisearch(mvm* m, char* key, int* n){
	int iCount = 0;
	char **c;
	int iIndex = 0;

	mvmcell *p;
	p = m->head;
	while (p != NULL){
		if (strcmp(p->key, key) == 0)
			iCount++;

		p = p->next;
	}

	*n = iCount;

	c = (char**)calloc(iCount, sizeof(char*));
	iIndex = 0;

	p = m->head;
	while (p != NULL){
		if (strcmp(p->key, key) == 0){
			c[iIndex] = p->data;
			iIndex++;
		}

		p = p->next;
	}

	return(c);
}

void mvm_free(mvm** p){
	mvmcell *current;
	mvmcell *next;

	current = (*p)->head;

	while (current != NULL){
		next = current->next;
		free(current->key);
		free(current->data);
		free(current);
		current = next;
	}

	(*p)->numkeys = 0;

	free(*p);
	*p = NULL;
}
