#ifndef SRC_H
#define SRC_H

// ---------- Estruturas de dados ------------

typedef struct arv_disciplina {
    int codigo_disciplina;
    char nome_disciplina[100];
    int periodo;
    int carga_horaria;
    struct arv_disciplina *esq;
    struct arv_disciplina *dir;
} Arv_Disciplina;

typedef struct arv_matricula {
    int codigo_disciplina;
    struct arv_matricula *esq;
    struct arv_matricula *dir;
} Arv_Matricula;

typedef struct arv_notas {
    int codigo_disciplina;
    int semestre;
    float nota_final;
    struct arv_notas *esq;
    struct arv_notas *dir;
} Arv_Notas;

typedef struct alunos {
    int matricula;
    char nome[100];
    int codigo_curso;
    Arv_Notas *notas;
    Arv_Matricula *matriculas;
    struct alunos *prox;
} Alunos;

typedef struct arv_cursos {
    int codigo_curso;
    char nome_curso[100];
    int quantidade_periodos;
    struct arv_cursos *esq;
    struct arv_cursos *dir;
    Arv_Disciplina *disciplina;
    Alunos *alunos;
} Arv_Cursos;

extern int contador_disciplina;

// ---------- Funções relacionadas a Cursos ------------
void gerar_codigo_disciplina(int *codigo_disciplina);
Arv_Cursos* localizar_curso(Arv_Cursos *curso, int codigo_curso);
void cadastrar_curso(Arv_Cursos **curso, int codigo_curso, int quantidade_periodos,const char *nome_curso);
void exibir_curso(Arv_Cursos *r);


// ---------- Funções relacionadas a Alunos ------------
void cadastrar_aluno(Alunos **a, int mat, char *nome, int codigo_curso);
void exibir_alunos_curso(Alunos *a, int codigo_curso);
Alunos* procurar_aluno_por_matricula_no_curso(Arv_Cursos *curso, int codigo_curso, int matricula);
void exibir_alunos(Alunos *lista);
void consultar_historico(Alunos *aluno, Arv_Cursos *curso, int matricula);

// ---------- Funções relacionadas a Matrículas ------------
void cadastrar_matricula(Alunos **a, int codigo, int mat);
void remover_matricula(Arv_Matricula **m, int cod);
void apresentar_matriculas(Arv_Matricula *r);
int verificar_matriculas_alunos(Alunos *aluno, int codigo_disciplina);

// ---------- Funções relacionadas a Notas ------------
int cadastrar_nota(Alunos **aluno, int matricula, int codigo, int semestre, float nota_final);
void exibir_nota_aluno_disciplina(Arv_Cursos *curso, Alunos *aluno, int codigo_disciplina, int matricula);
void notas_disciplina_periodo_aluno(Alunos *aluno, int periodo, int matricula);
int notas_disciplina_periodo(Arv_Notas *notas, int periodo);
void mostrar_notas_aluno(Alunos *aluno, Arv_Disciplina *disciplinas, int periodo);


// ---------- Funções Referentes a Disciplina ------------
void exibir_disciplina_periodo(Arv_Cursos *curso, int periodo);
int cadastrar_disciplina(Arv_Cursos **curso, Arv_Disciplina *No, int codigo_curso);
void exibir_disciplinasporcurso(Arv_Cursos *curso, int codigo_curso);
int remover_disciplina_curso(Arv_Cursos **cursos, Alunos *alunos, int idcurso, int codigo_disciplina);
void exibir_disciplinas_aluno_especifico(Alunos *aluno, Arv_Cursos *cursos, int matricula);


// ---------- Escopos para funcionamento do src.c ------------
void remover_no_folha(Arv_Matricula **raiz, Arv_Matricula *pai, Arv_Matricula *atual);
void remover_no_com_um_filho(Arv_Matricula **raiz, Arv_Matricula *pai, Arv_Matricula *atual);
void remover_no_com_dois_filhos(Arv_Matricula *atual);
Alunos *buscar_aluno(Alunos *aluno, int matricula);
void procurar_matricula(Arv_Matricula *r, int codigo, int *encontrado);


#endif

