#include "src.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

int contador_disciplina = 1; // não reenicia
// ---------------------------------------------------- XXXXXX -------------------------------------------------
// I - Cadastrar alunos a qualquer momento na lista, de forma que só possa cadastrar um código de curso que
// já tenha sido cadastrado na árvore de cursos.
// ---------------------------------------------------- XXXXXX -------------------------------------------------
void converter_nome_maiusculas(char *nome){
    int i = 0;
    while (nome[i] != '\0'){
        nome[i] = toupper(nome[i]);
        i++;
    }
}

void cadastrar_aluno(Alunos **aluno, int matricula, char *nome, int codigo_curso){
    Alunos *novo = (Alunos *)malloc(sizeof(Alunos));
    if (novo == NULL) return; // Verifica a alocação de memória

    novo->prox = NULL;
    novo->matricula = matricula;

    char *aux_nome = strdup(nome); // Duplicando o nome
    converter_nome_maiusculas(aux_nome);
    strcpy(novo->nome, aux_nome);
    free(aux_nome); // Libera a memória do nome duplicado

    novo->codigo_curso = codigo_curso;
    novo->notas = NULL;

    if (*aluno == NULL) 
        *aluno = novo; // Insere o novo aluno na lista vazia
    else {
        if (strcmp(novo->nome, (*aluno)->nome) < 0) {
            novo->prox = *aluno; // Insere no início
            *aluno = novo;
        }
        else {
            Alunos *aux = *aluno; 
            while (aux->prox != NULL && strcmp(novo->nome, aux->prox->nome) > 0)
                aux = aux->prox;
            novo->prox = aux->prox; 
            aux->prox = novo; // Insere o novo aluno na posição correta
        }
    }
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// II - Cadastrar cursos a qualquer momento na árvore de curso, de forma que o usuário não precise cadastrar
// as disciplinas para permitir o cadastro do curso.
// ---------------------------------------------------- XXXXXX -------------------------------------------------

void cadastrar_curso(Arv_Cursos **curso, int codigo_curso, int quantidade_periodos,const char *nome_curso)
{
    if (*curso == NULL) {
        Arv_Cursos *novo = (Arv_Cursos *)malloc(sizeof(Arv_Cursos));
        if (novo == NULL) return; // Verifica se a alocação de memória foi bem-sucedida

        novo->codigo_curso = codigo_curso;
        strcpy(novo->nome_curso, nome_curso);
        novo->quantidade_periodos = quantidade_periodos;
        novo->esq = NULL; // Inicializa o filho esquerdo como NULL
        novo->dir = NULL; // Inicializa o filho direito como NULL
        *curso = novo; // Insere o novo nó na árvore
    }
    else {
        // Recursão para inserir o curso na posição correta na árvore
        if (codigo_curso < (*curso)->codigo_curso)
            cadastrar_curso(&((*curso)->esq), codigo_curso, quantidade_periodos, nome_curso);
        else
            cadastrar_curso(&((*curso)->dir), codigo_curso, quantidade_periodos, nome_curso);
    }
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// III -  Cadastrar disciplinas a qualquer momento em uma árvore de disciplinas de um determinado curso, ou
// seja, um disciplina só pode ser cadastrada se o curso já estiver sido cadastrado, além disso, o período da
// disciplina deve ser válido, ou seja, estar entre 1 e a quantidade máxima de períodos do curso. A carga
// horária da disciplina deve ser múltiplo de 15, variando entre 30 e 90.
// ---------------------------------------------------- XXXXXX -------------------------------------------------

void validar_carga_horaria(int *resultado_validacao, int carga_horaria) {
    if (carga_horaria >= 30 && carga_horaria <= 90 && carga_horaria % 15 == 0) // Tem que ser 30, 45, 60, 75 ou 90.
        *resultado_validacao = 1; // Define resultado como 1 se a carga horária for válida
}

void validar_periodo(Arv_Cursos *curso, int *resultado_validacao, int periodo){
    if (periodo >= 1 && periodo <= curso->quantidade_periodos)
        *resultado_validacao = 1; // Define resultado como 1 se o número do período for válido
}

// Insere uma nova disciplina na árvore de disciplinas, mantendo a ordem por código da disciplina.
void add_disciplina_na_arvore(Arv_Disciplina **disciplina, Arv_Disciplina *No, int *insere) {
    if (*disciplina == NULL) { // Está vaiza
        *disciplina = No;
        No->esq = NULL; // Inicializa o filho esquerdo como NULL
        No->dir = NULL; // Inicializa o filho direito como NULL
        *insere = 1; // Indica que a inserção foi bem-sucedida
    }
    else if (No->codigo_disciplina < (*disciplina)->codigo_disciplina) 
        add_disciplina_na_arvore(&((*disciplina)->esq), No, insere); // Insere no filho esquerdo
    else if (No->codigo_disciplina > (*disciplina)->codigo_disciplina) 
        add_disciplina_na_arvore(&((*disciplina)->dir), No, insere); // Insere no filho direito
}



// Cadastra uma nova disciplina em um curso específico, validando carga horária e período.
int cadastrar_disciplina(Arv_Cursos **curso, Arv_Disciplina *No, int codigo_curso)
{
    int validacao_carga_horaria = 0, validacao_periodo = 0, sucesso = 0;
    
    validar_carga_horaria(&validacao_carga_horaria, No->carga_horaria);

    if (validacao_carga_horaria == 1 && *curso != NULL){ // Verifica carga horária e se o curso existe
    
        if ((*curso)->codigo_curso == codigo_curso) {
            validar_periodo(*curso, &validacao_periodo, No->periodo);
            if (validacao_periodo == 1) {
                add_disciplina_na_arvore(&((*curso)->disciplina), No, &sucesso);
                if (sucesso != 0) 
                    sucesso = 1; 
            }
        }
        else {
            // Chama recursivamente para buscar na subárvore apropriada
            sucesso = (codigo_curso < (*curso)->codigo_curso) 
                        ? cadastrar_disciplina(&(*curso)->esq, No, codigo_curso) 
                        : cadastrar_disciplina(&(*curso)->dir, No, codigo_curso);
        }
    }

    return sucesso; // Retorna 1 se a disciplina foi cadastrada com sucesso
}




void gerar_codigo_para_disciplina(int *codigo_disciplina) {
    *codigo_disciplina = contador_disciplina;
    contador_disciplina++;
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// IV - Cadastrar uma matrícula, onde a mesma é uma árvore organizada e contendo somente um código de
// uma disciplina do curso do aluno.
// ---------------------------------------------------- XXXXXX -------------------------------------------------
void inserir_matricula(Arv_Matricula **matricula, int codigo, int *existe) {
    if (*matricula == NULL) {
        Arv_Matricula *novo = (Arv_Matricula *)malloc(sizeof(Arv_Matricula));
        novo->codigo_disciplina = codigo;
        novo->esq = NULL;
        novo->dir = NULL;
        *matricula = novo;
    } else if (codigo < (*matricula)->codigo_disciplina) {
        inserir_matricula(&(*matricula)->esq, codigo, existe); 
    } else if (codigo > (*matricula)->codigo_disciplina) {
        inserir_matricula(&(*matricula)->dir, codigo, existe); 
    } else {
        *existe = -1; // Sinaliza que o código já existe
    }
}

void cadastrar_matricula(Alunos **aluno, int codigo, int matricula) {
    if (*aluno != NULL) {
        if ((*aluno)->matricula == matricula) {  // Verifica se o aluno tem o número de matrícula fornecido
            int ja_existe = 0;
            // Insere a disciplina/matrícula na árvore de matrículas do aluno
            inserir_matricula(&(*aluno)->matricula, codigo, &ja_existe); // Usa o campo correto 'matriculas'
        } else {
            // Se a matrícula do aluno atual não é a correta, continua buscando no próximo aluno
            cadastrar_matricula(&(*aluno)->prox, codigo, matricula);
        }
    }
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// V - Cadastrar Notas, permitir o cadastro de notas somente de disciplinas que estejam na árvore de
// matricula, e quando a nota for cadastrada a disciplina deve ser removida da árvore de matricula para
// árvore de notas.
// ---------------------------------------------------- XXXXXX -------------------------------------------------

// Verifica se achou a diciplina ou não
void verificar_existencia_disciplina(Arv_Matricula *matricula, int codigo, int *achou_disciplina){
    if (matricula != NULL)
    {
        if (matricula->codigo_disciplina == codigo)
            *achou_disciplina = 1;
        else if (codigo < matricula->codigo_disciplina)
            verificar_existencia_disciplina(matricula->esq, codigo, achou_disciplina);
        else
            verificar_existencia_disciplina(matricula->dir, codigo, achou_disciplina);
    }
}


// Busca um aluno na lista encadeada de alunos com base na matrícula
Alunos *buscar_aluno(Alunos *aluno, int matricula){
    // Percorre a lista enquanto o aluno não for encontrado ou a lista não terminar
    while (aluno != NULL && aluno->matricula != matricula)
        aluno = aluno->prox;

    // Retorna o aluno encontrado ou NULL se não encontrar
    return aluno;
}


// Cadastra uma nova nota na árvore de notas com base no código da disciplina e semestre
void cadastrar_nota_aux(Arv_Notas **nota, int codigo, float nota_final, int semestre) {
    if (*nota == NULL) { // Se o nó atual for nulo, cria um novo nó
        Arv_Notas *novo = (Arv_Notas *)malloc(sizeof(Arv_Notas));
        novo->codigo_disciplina = codigo;
        novo->semestre = semestre;
        novo->nota_final = nota_final;
        novo->esq = NULL;
        novo->dir = NULL;
        *nota = novo; // Atualiza o ponteiro para apontar para o novo nó
    } else {
        if (codigo < (*nota)->codigo_disciplina) 
            cadastrar_nota_aux(&((*nota)->esq), codigo, nota_final, semestre); 
        else 
            cadastrar_nota_aux(&((*nota)->dir), codigo, nota_final, semestre); 
    }
}


// Cadastra uma nota para um aluno específico se a disciplina correspondente for encontrada
int cadastrar_nota(Alunos **aluno, int matricula, int codigo, int semestre, float nota_final) {
    int resultado = 0; // Variável que indica se a nota foi cadastrada com sucesso

    if (*aluno != NULL) {
        if ((*aluno)->matricula == matricula) {
            int disciplina_encontrada = 0; 
            verificar_existencia_disciplina((*aluno)->matricula, codigo, &disciplina_encontrada);
            if (disciplina_encontrada == 1) {
                remover_matricula(&(*aluno)->matricula, codigo);
                cadastrar_nota_aux(&(*aluno)->notas, codigo, nota_final, semestre); 
                resultado = 1; // Indica que a nota foi cadastrada com sucesso
            }
        } else {
            resultado = cadastrar_nota(&(*aluno)->prox, matricula, codigo, semestre, nota_final); // Chamada recursiva para o próximo aluno
        }
    }

    return resultado; // Retorna 1 se a nota foi cadastrada, caso contrário, retorna 0
}


// ---------------------------------------------------- XXXXXX -------------------------------------------------
// VI -  Mostrar todos os alunos de um determinado curso.
// ---------------------------------------------------- XXXXXX -------------------------------------------------
// Exibe os alunos de um curso específico com base no código do curso
void exibir_alunos_curso(Alunos *aluno, int codigo_curso) {
    if (aluno != NULL) {
        if (aluno->codigo_curso == codigo_curso) {
            printf("Nome do Aluno: %s\n", aluno->nome);
            printf("Matricula: %d\n", aluno->matricula);
            printf("\n");
        }
        exibir_alunos_curso(aluno->prox, codigo_curso); // Chamada recursiva para o próximo aluno
    }
}


// ---------------------------------------------------- XXXXXX -------------------------------------------------
// VII - Mostrar todos os cursos do Campus
// ---------------------------------------------------- XXXXXX -------------------------------------------------
// Exibe informações de um curso em uma árvore binária de cursos
void exibir_curso(Arv_Cursos *curso) {
    if (curso != NULL) { // Verifica se o nó atual não é nulo
        printf("NOME DO CURSO: %s | ID: %d\n", curso->nome_curso, curso->codigo_curso);
        exibir_curso(curso->esq); // Chama a função recursivamente para o filho à esquerda
        exibir_curso(curso->dir); // Chama a função recursivamente para o filho à direita
    }
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// VIII - Mostrar todas as disciplinas de um determinado curso.
// ---------------------------------------------------- XXXXXX -------------------------------------------------


// Exibe as disciplinas de um curso de forma recursiva
void exibir_disciplinas_de_um_curso_recursivamente(Arv_Disciplina *disciplina) {
    if (disciplina != NULL) { // Verifica se o nó atual não é nulo

        // Exibe as informações da disciplina
        printf("Código: %d\n", disciplina->codigo_disciplina);
        printf("Nome: %s\n", disciplina->nome_disciplina);
        printf("Período: %d\n", disciplina->periodo);
        printf("Carga Horária: %d\n", disciplina->carga_horaria);
        printf("-------------------------------\n");

        // Chamada recursiva para as subárvores esquerda e direita
        exibir_disciplinas_de_um_curso_recursivamente(disciplina->esq);
        exibir_disciplinas_de_um_curso_recursivamente(disciplina->dir);
    }
}

void exibir_disciplinasporcurso(Arv_Cursos *curso, int codigo_curso)
{
    if (curso != NULL)
    {
        if (curso->codigo_curso == codigo_curso)
        {
            Arv_Disciplina *disciplina_atual = curso->disciplina;
            printf("Disciplinas do curso %s:\n", curso->nome_curso);
            printf("-------------------------------\n");

            exibir_disciplinasporcurso_recursivamente(disciplina_atual);
        }

        exibir_disciplinasporcurso(curso->esq, codigo_curso);
        exibir_disciplinasporcurso(curso->dir, codigo_curso);
    }
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// IX - Mostrar todas as disciplinas de um determinado período de um curso.
// ---------------------------------------------------- XXXXXX -------------------------------------------------
// Exibe as disciplinas de um determinado período nos cursos
void exibir_disciplina_periodo(Arv_Cursos *curso, int periodo) {
    if (curso != NULL) {
        if (curso->disciplina->periodo == periodo) {
            printf("Curso: %s | Disciplina: %s | Período: %d\n", curso->nome_curso, curso->disciplina->nome_disciplina, curso->disciplina->periodo);
        }
        exibir_disciplina_periodo(curso->esq, periodo);
        exibir_disciplina_periodo(curso->dir, periodo);
    }
}


// ---------------------------------------------------- XXXXXX -------------------------------------------------
// X - Mostrar todas as disciplinas que um determinado aluno está matriculado
// ---------------------------------------------------- XXXXXX -------------------------------------------------

// Exibe as informações de uma disciplina específica
void exibir_disciplinas(Arv_Disciplina *disciplina_atual, int codigo_disciplina) {
    if (disciplina_atual != NULL) {
        if (disciplina_atual->codigo_disciplina == codigo_disciplina) {
            printf("Código: %d\n", disciplina_atual->codigo_disciplina);
            printf("Nome: %s\n", disciplina_atual->nome_disciplina);
            printf("Carga Horária: %d\n", disciplina_atual->carga_horaria);
            printf("Período: %d\n", disciplina_atual->periodo);
            printf("\n");
        } else if (codigo_disciplina < disciplina_atual->codigo_disciplina) {
            exibir_disciplinas(disciplina_atual->esq, codigo_disciplina);
        } else {
            exibir_disciplinas(disciplina_atual->dir, codigo_disciplina);
        }
    }
}


// Exibe as disciplinas de um curso específico
// Exibe a disciplina de um curso específico
void exibir_disciplina_do_curso(Arv_Cursos *curso_atual, int codigo_disciplina, int codigo_curso) {
    if (curso_atual != NULL) {
        if (curso_atual->codigo_curso == codigo_curso) {
            if (curso_atual->disciplina != NULL) {
                exibir_disciplinas(curso_atual->disciplina, codigo_disciplina);
            }
        } else if (codigo_curso < curso_atual->codigo_curso) {
            exibir_disciplina_do_curso(curso_atual->esq, codigo_disciplina, codigo_curso);
        } else {
            exibir_disciplina_do_curso(curso_atual->dir, codigo_disciplina, codigo_curso);
        }
    }
}

// Exibe as disciplinas que um aluno está matriculado
void exibir_disciplina_aluno(Arv_Matricula *matricula, Arv_Cursos *cursos, int codigo_curso) {
    if (matricula != NULL) {
        exibir_disciplina(cursos, matricula->codigo_disciplina, codigo_curso); // Exibe a disciplina do aluno
        exibir_disciplina_aluno(matricula->esq, cursos, codigo_curso); // Chamada recursiva para a subárvore esquerda
        exibir_disciplina_aluno(matricula->dir, cursos, codigo_curso); // Chamada recursiva para a subárvore direita
    }
}

// Exibe as disciplinas de um aluno específico dado seu número de matrícula
void exibir_disciplinas_aluno_especifico(Alunos *aluno, Arv_Cursos *cursos, int matricula) {
    if (aluno != NULL) {
        if (aluno->matricula == matricula)
            exibir_disciplina_aluno(aluno->matricula, cursos, aluno->codigo_curso); // Exibe as disciplinas do aluno
        else
            exibir_disciplinas_aluno_especifico(aluno->prox, cursos, matricula); // Chamada recursiva para o próximo aluno
    }
}


// ---------------------------------------------------- XXXXXX -------------------------------------------------
// XI -  Mostrar todas as notas de disciplinas de um determinado período de um determinado aluno.
// ---------------------------------------------------- XXXXXX -------------------------------------------------

void exibir_notas_por_periodo(Arv_Notas *nota, int periodo)
{
    if (nota == NULL) // Verifica se o nó atual é nulo
        return; // Se for nulo, sai da função

    // Se o semestre da nota for igual ao período especificado
    if (nota->semestre == periodo)
    {
        printf("Codigo: %d\n", nota->codigo_disciplina);
        printf("Nota Final: %.2f\n", nota->nota_final);
        printf("Semestre: %.2f\n", nota->semestre);
        printf("\n");
    }
    
    // Chamada recursiva para as subárvores esquerda e direita
    exibir_notas_por_periodo(nota->esq, periodo);
    exibir_notas_por_periodo(nota->dir, periodo);
}


void notas_disciplina_periodo_aluno(Alunos *aluno, int periodo, int matricula)
{
    if (aluno != NULL)
    {
        if (aluno->matricula == matricula)
            exibir_notas_por_periodo(aluno->notas, periodo);
        else
            notas_disciplina_periodo_aluno(aluno->prox, periodo, matricula);
    }
}

// ---------------------------------------------------- XXXXXX -------------------------------------------------
// xii) Mostrar a nota de uma disciplina de um determinado aluno, mostrando o período e a carga horária da disciplina
// ---------------------------------------------------- XXXXXX -------------------------------------------------

void exibir_nota_aluno_disciplina(Arv_Cursos *curso, Alunos *aluno, int codigo_disciplina, int matricula)
{
    if (aluno != NULL)
    {
        if (aluno->matricula == matricula)
        {
            Arv_Notas *nota = aluno->notas;
            while (nota != NULL)
            {
                if (nota->codigo_disciplina == codigo_disciplina)
                {
                    Arv_Disciplina *disciplina = curso->disciplina;
                    while (disciplina != NULL)
                    {
                        if (disciplina->codigo_disciplina == codigo_disciplina)
                        {
                            // Exibe as informações do aluno e nota
                            printf("Aluno: %s\nDisciplina: %d\nPeriodo: %d\nCH: %d\nNota Final: %.2f\n",
                                   aluno->nome, nota->codigo_disciplina, disciplina->periodo, disciplina->carga_horaria, nota->nota_final);
                            // Após exibir, podemos sair do loop das disciplinas
                            disciplina = NULL; // Atribuindo NULL para sair do loop
                        }
                        else
                        {
                            disciplina = (codigo_disciplina < disciplina->codigo_disciplina) ? disciplina->esq : disciplina->dir; // Navega na árvore de disciplinas
                        }
                    }
                }
                nota = (codigo_disciplina < nota->codigo_disciplina) ? nota->esq : nota->dir; // Navega na árvore de notas
            }
        }
        else
        {
            exibir_nota_aluno_disciplina(curso, aluno->prox, codigo_disciplina, matricula); // Chamada recursiva para o próximo aluno
        }
    }
}


// ---------------------------------------------------- XXXXXX -------------------------------------------------
// XIII - Remover uma disciplina de um determinado curso desde que não tenha nenhum aluno matriculado na
// mesma.
// ---------------------------------------------------- XXXXXX -------------------------------------------------
int e_folha(Arv_Disciplina *disciplina){
    return (disciplina->esq == NULL && disciplina->dir == NULL);
}

Arv_Disciplina *so_um_filho(Arv_Disciplina *disciplina) {
    Arv_Disciplina *aux = NULL;

    if (disciplina != NULL) {
        if (disciplina->esq == NULL) {
            aux = disciplina->dir; // Se não tem filho esquerdo, pega o direito
        } else if (disciplina->dir == NULL) {
            aux = disciplina->esq; // Se não tem filho direito, pega o esquerdo
        }
    }

    return aux; // Retorna o filho ou NULL se não tiver apenas um filho
}

Arv_Disciplina *menor_filho_esquerda(Arv_Disciplina *disciplina) {
    Arv_Disciplina *aux = disciplina; // Inicializa com a disciplina atual

    while (aux != NULL && aux->esq != NULL) {
        aux = aux->esq; // Navega para o filho esquerdo
    }

    return aux; // Retorna o menor filho à esquerda ou NULL se não houver
}

void remover_disciplina(Arv_Disciplina **disciplina, int codigo_disciplina, int *remove) {
    if (*disciplina != NULL) {
        Arv_Disciplina *aux;
        Arv_Disciplina *endereco_filho = so_um_filho(*disciplina);
        Arv_Disciplina *endereco_menor_filho;

        if ((*disciplina)->codigo_disciplina == codigo_disciplina) {
            if (e_folha(*disciplina)) {
                aux = *disciplina;
                free(aux);
                *disciplina = NULL;
            } else if (endereco_filho != NULL) {
                aux = *disciplina;
                free(aux);
                *disciplina = endereco_filho;
            } else {
                endereco_menor_filho = menor_filho_esquerda((*disciplina)->dir);
                (*disciplina)->codigo_disciplina = endereco_menor_filho->codigo_disciplina;
                (*disciplina)->carga_horaria = endereco_menor_filho->carga_horaria;
                (*disciplina)->periodo = endereco_menor_filho->periodo;
                strcpy((*disciplina)->nome_disciplina, endereco_menor_filho->nome_disciplina);
                remover_disciplina(&(*disciplina)->dir, endereco_menor_filho->codigo_disciplina, remove);
            }
            *remove = 1;
        } else {
            if (codigo_disciplina < (*disciplina)->codigo_disciplina) {
                remover_disciplina(&(*disciplina)->esq, codigo_disciplina, remove);
            } else {
                remover_disciplina(&(*disciplina)->dir, codigo_disciplina, remove);
            }
        }
    }
}

void confirmar_remocao_disciplina(Alunos *alunos, int codigo_disciplina, int *sim_ou_nao) {
    int achou = 0;

    if (alunos != NULL) {
        procurar_matricula(alunos->matricula, codigo_disciplina, &achou);

        if (achou != 0)
            *sim_ou_nao = 1;

        confirmar_remocao_disciplina(alunos->prox, codigo_disciplina, sim_ou_nao);
    }
}


int remover_disciplina_curso(Arv_Cursos **cursos, Alunos *alunos, int id_curso, int codigo_disciplina)
{
    int resultado_remocao = 0; // Renomeado de 'removido' para 'resultado_remocao'
    int disciplina_encontrada = 0; // Renomeado de 'disciplina_confirmada' para 'disciplina_encontrada'
    
    if ((*cursos))
    {
        confirmar_remocao_disciplina(alunos, codigo_disciplina, &disciplina_encontrada);
        
        if (disciplina_encontrada)
        {
            if ((*cursos)->codigo_curso == id_curso)
                remover_disciplina(&(*cursos)->disciplina, codigo_disciplina, &resultado_remocao);
            else if (id_curso < (*cursos)->codigo_curso)
                resultado_remocao = remover_disciplina_curso(&(*cursos)->esq, alunos, id_curso, codigo_disciplina);
            else
                resultado_remocao = remover_disciplina_curso(&(*cursos)->dir, alunos, id_curso, codigo_disciplina);
        }
    }
    return resultado_remocao; // Retorno simplificado
}



// ---------------------------------------------------- XXXXXX -------------------------------------------------
// XIV - Permita remover uma disciplina da árvore de matrícula de um determinado aluno.
// ---------------------------------------------------- XXXXXX -------------------------------------------------

int e_folha_matricula(Arv_Matricula *matricula){
    return (matricula->esq == NULL && matricula->dir == NULL);
}

Arv_Matricula *um_filho_matricula(Arv_Matricula *matricula)
{
    Arv_Matricula *filho = NULL;

    if (matricula->esq == NULL) // Se não há filho à esquerda
        filho = matricula->dir; // Atribui o filho à direita
    else if (matricula->dir == NULL) // Se não há filho à direita
        filho = matricula->esq; // Atribui o filho à esquerda

    return filho; // Retorna o filho encontrado
}

Arv_Matricula *menor_filho_esquerdo(Arv_Matricula *matricula)
{
    Arv_Matricula *resultado = NULL;

    if (matricula) // Verifica se o nó atual não é NULL
    {
        resultado = menor_filho_esquerdo(matricula->esq); // Chamada recursiva para o filho à esquerda
        if (!resultado) // Se não encontrou menor à esquerda
            resultado = matricula; // Atribui o nó atual como resultado
    }
    
    return resultado; // Retorna o menor encontrado ou NULL
}


void remover_matricula(Arv_Matricula **matricula, int codigo)
{
    if (*matricula != NULL)
    {
        if ((*matricula)->codigo_disciplina == codigo)
        {
            Arv_Matricula *aux;

            // Verifica se o nó é uma folha
            if (e_folha_matricula(*matricula))
            {
                aux = *matricula;
                free(aux);
                *matricula = NULL;
            }
            // Verifica se tem apenas um filho
            else if ((aux = um_filho_matricula(*matricula)) != NULL)
            {
                Arv_Matricula *temp;
                temp = *matricula;
                free(temp);
                *matricula = aux;
            }
            // Caso com dois filhos
            else
            {
                Arv_Matricula *menorfilho = menor_filho_esquerdo((*matricula)->dir);
                (*matricula)->codigo_disciplina = menorfilho->codigo_disciplina;
                remover_matricula(&(*matricula)->dir, menorfilho->codigo_disciplina);
            }
        }
        else if (codigo < (*matricula)->codigo_disciplina)
            remover_matricula(&(*matricula)->esq, codigo);
        else
            remover_matricula(&(*matricula)->dir, codigo);
    }
}



// ---------------------------------------------------- XXXXXX -------------------------------------------------
// XV - Mostrar o histórico de um determinado aluno, contendo o nome do curso, as disciplinas e sua respectiva
// nota organizadas pelo período que a disciplina está cadastrada no curso.
// ---------------------------------------------------- XXXXXX -------------------------------------------------

void exibir_disciplina_historico(Arv_Disciplina *disciplina, int codigo_disciplina){
    if(disciplina != NULL)
    {
        if(disciplina->codigo_disciplina == codigo_disciplina) 
            printf("Disciplina: %s\n", disciplina->nome_disciplina);
        else if(codigo_disciplina < disciplina->codigo_disciplina)
            exibir_disciplina_historico(disciplina->esq, codigo_disciplina);
        else
            exibir_disciplina_historico(disciplina->dir, codigo_disciplina);
    }
}

void exibir_notas_alunos(Arv_Notas *nota, Arv_Disciplina *disciplina, int periodo) {
    if (nota != NULL) {
        if (nota->semestre == periodo) {
            exibir_disciplina_historico(disciplina, nota->codigo_disciplina);
            printf("\n--------------\n");
            printf("Semestre: %.1f\n", nota->semestre);
            printf("Nota: %.2f\n", nota->nota_final);
            printf("\n--------------\n");
        }
        exibir_notas_alunos(nota->esq, disciplina, periodo);  // Chamada recursiva para a subárvore esquerda
        exibir_notas_alunos(nota->dir, disciplina, periodo);  // Chamada recursiva para a subárvore direita
    }
}


void consultar_historico(Alunos *aluno, Arv_Cursos *curso, int matricula)
{
    if(aluno != NULL)
    {
        if(aluno->matricula == matricula)
        {
            printf("\n------------------\n");
            printf("Matricula: %d\n", aluno->matricula);
            printf("Aluno: %s\n", aluno->nome);
            printf("\n------------------\n");
            int count_periodos = 0;
            count_periodos = exibir_nome_do_curso(curso, aluno->codigo_curso);
            printf("Historico:\n");
            for (int i = 0; i < count_periodos; i++)
                exibir_notas_alunos(aluno->notas, curso->disciplina, i+1);
        }
        else
            consultar_historico(aluno->prox, curso, matricula);
    }
}




// ---------------------------------------------------- XXXXXX -------------------------------------------------
// FUNÇÕES AUXILIARES
// ---------------------------------------------------- XXXXXX -------------------------------------------------

Arv_Cursos *localizar_curso(Arv_Cursos *curso, int codigo_curso)
{
    Arv_Cursos *curso_encontrado = NULL; // Variável renomeada para clareza

    if (curso != NULL)
    {
        if (curso->codigo_curso == codigo_curso)
            curso_encontrado = curso; // Resultado encontrado
        else if (codigo_curso < curso->codigo_curso)
            curso_encontrado = localizar_curso(curso->esq, codigo_curso); // Busca na subárvore esquerda
        else
            curso_encontrado = localizar_curso(curso->dir, codigo_curso); // Busca na subárvore direita
    }

    return curso_encontrado; // Retorna o curso encontrado ou NULL
}


void apresentar_matriculas(Arv_Matricula *raiz)
{
    if (raiz != NULL)
    {
        printf("Matricula: %d\n", raiz->codigo_disciplina); // Exibe a matrícula atual
        apresentar_matriculas(raiz->esq); // Chama recursivamente para a subárvore esquerda
        apresentar_matriculas(raiz->dir); // Chama recursivamente para a subárvore direita
    }
}


void procurar_matricula(Arv_Matricula *raiz, int codigo, int *encontrado)
{
    if (raiz != NULL)
    {
        if (raiz->codigo_disciplina == codigo)
            *encontrado = 1; // Matricula encontrada
        else
        {
            if (codigo < raiz->codigo_disciplina)
                procurar_matricula(raiz->esq, codigo, encontrado); // Busca na subárvore esquerda
            else
                procurar_matricula(raiz->dir, codigo, encontrado); // Busca na subárvore direita
        }
    }
}


Alunos *procurar_aluno_por_matricula_no_curso(Arv_Cursos *curso, int codigo_curso, int matricula)
{
    Alunos *aluno = NULL; // Inicializa aluno como NULL
    if (curso != NULL)
    {
        if (curso->codigo_curso == codigo_curso)
        {
            aluno = curso->alunos; // Acessa a lista de alunos do curso
            while (aluno != NULL && aluno->matricula != matricula)
                aluno = aluno->prox; // Percorre a lista de alunos
        }
        else
        {
            aluno = procurar_aluno_por_matricula_no_curso(curso->esq, codigo_curso, matricula); // Busca na subárvore esquerda
            if (aluno == NULL)
                aluno = procurar_aluno_por_matricula_no_curso(curso->dir, codigo_curso, matricula); // Busca na subárvore direita
        }
    }
    return aluno; // Retorna o aluno encontrado ou NULL
}


int notas_disciplina_periodo(Arv_Notas *n, int periodo)
{
    if (n == NULL)
        printf("Nota nao encontrada\n"); // Mensagem caso a nota não seja encontrada
    else
    {
        if (n->semestre == periodo)
            printf("Disciplina: %d, Nota Final: %.2f\n", n->codigo_disciplina, n->nota_final); // Exibe nota se o semestre corresponder

        notas_disciplina_periodo(n->esq, periodo); // Busca na subárvore esquerda
        notas_disciplina_periodo(n->dir, periodo); // Busca na subárvore direita
    }
}


void procurar_no(Arv_Matricula **atual, Arv_Matricula **pai, int codigo_disciplina, int *encontrado)
{
    while (*atual != NULL && *encontrado == 0) // Continua até encontrar ou atingir o final da árvore
    {
        if ((*atual)->codigo_disciplina == codigo_disciplina)
            *encontrado = 1; // Marca como encontrado se o código corresponder
        else
        {
            *pai = *atual; // Atualiza o nó pai

            if (codigo_disciplina < (*atual)->codigo_disciplina)
                *atual = (*atual)->esq; // Move para a subárvore esquerda
            else
                *atual = (*atual)->dir; // Move para a subárvore direita
        }
    }
}


void remover_no(Arv_Matricula **raiz, Arv_Matricula *pai, Arv_Matricula *atual)
{
    // Verifica se o nó atual é uma folha
    if (atual->esq == NULL && atual->dir == NULL)
        remover_no_folha(raiz, pai, atual); // Remove o nó folha

    // Verifica se o nó atual tem apenas um filho
    else if (atual->esq == NULL || atual->dir == NULL)
        remover_no_com_um_filho(raiz, pai, atual); // Remove o nó com um filho
    else
        remover_no_com_dois_filhos(atual); // Remove o nó com dois filhos

    free(atual); // Libera a memória do nó atual
}


void remover_no_folha(Arv_Matricula **raiz, Arv_Matricula *pai, Arv_Matricula *atual)
{
    // Verifica se o nó atual é a raiz
    if (pai == NULL)
        *raiz = NULL; // Se for a raiz, a árvore fica vazia

    else if (pai->esq == atual)
        pai->esq = NULL; // Remove o nó da esquerda
    else
        pai->dir = NULL; // Remove o nó da direita
}


void remover_no_com_um_filho(Arv_Matricula **raiz, Arv_Matricula *pai, Arv_Matricula *atual)
{
    Arv_Matricula *filho;
    if (atual->esq != NULL) 
        filho = atual->esq;
    else 
        filho = atual->dir;
    

    if (pai == NULL)
        *raiz = filho;

    else if (pai->esq == atual)
        pai->esq = filho;

    else
        pai->dir = filho;
}


void remover_no_com_dois_filhos(Arv_Matricula *no_atual){
    Arv_Matricula *no_menor = no_atual->dir; // Inicia a busca pelo menor nó na subárvore direita
    Arv_Matricula *pai_menor = no_atual;

    // Encontra o menor nó da subárvore direita
    while (no_menor->esq != NULL)
    {
        pai_menor = no_menor;
        no_menor = no_menor->esq;
    }

    no_atual->codigo_disciplina = no_menor->codigo_disciplina; // Substitui o código do nó atual pelo do menor nó encontrado

    // Ajusta a ligação do pai do menor nó
    if (pai_menor->esq == no_menor)
        pai_menor->esq = no_menor->dir; // Remove o menor nó da subárvore
    else
        pai_menor->dir = no_menor->dir;
}


Arv_Disciplina *procurar_disciplina(Arv_Disciplina *disciplina, int codigo_disciplina)
{
    while (disciplina != NULL && disciplina->codigo_disciplina != codigo_disciplina)
    {
        if (codigo_disciplina > disciplina->codigo_disciplina)
            disciplina = disciplina->dir;
        else
            disciplina = disciplina->esq;
    }
    return disciplina;
}

Arv_Notas *procurar_nota(Arv_Notas *no_nota, int codigo_disciplina)
{
    while (no_nota != NULL && no_nota->codigo_disciplina != codigo_disciplina)
    {
        // Navega na árvore baseada na comparação do código da disciplina
        if (codigo_disciplina > no_nota->codigo_disciplina)
            no_nota = no_nota->dir;
        else
            no_nota = no_nota->esq;
    }
    return no_nota; // Retorna o nó encontrado ou NULL
}


void exibir_informacoes(Arv_Disciplina *disciplina_atual, Arv_Notas *nota_atual)
{
    printf("Disciplina: %s\n", disciplina_atual->nome_disciplina);
    printf("Periodo: %d\n", disciplina_atual->periodo);
    printf("Carga Horaria: %dh\n", disciplina_atual->carga_horaria);
    printf("Nota final: %.2f\n", nota_atual->nota_final);
}

void exibir_alunos(Alunos *lista)
{
    Alunos *aluno_atual = lista;

    while (aluno_atual != NULL)
    {
        printf("Matricula: %d\n", aluno_atual->matricula);
        printf("Nome: %s\n", aluno_atual->nome);
        printf("Codigo do curso: %d\n", aluno_atual->codigo_curso);

        printf("\n");
        aluno_atual = aluno_atual->prox;
    }
}

Arv_Disciplina *buscar_disciplina_por_codigo(Arv_Disciplina *disciplinas, int codigo)
{
    if (disciplinas == NULL) // Retorna NULL se a árvore estiver vazia
        return NULL;

    if (codigo == disciplinas->codigo_disciplina)
        return disciplinas; // Retorna o nó encontrado

    // Chamada recursiva para buscar à esquerda ou direita
    return (codigo < disciplinas->codigo_disciplina) ?
        buscar_disciplina_por_codigo(disciplinas->esq, codigo) :
        buscar_disciplina_por_codigo(disciplinas->dir, codigo);
}
