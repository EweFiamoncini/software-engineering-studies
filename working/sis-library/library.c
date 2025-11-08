#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// --- PARTE 1: ESTRUTURAS DE DADOS E CONSTANTES ---

// Constantes para limites
#define MAX_LIVROS 100
#define MAX_USUARIOS 100
#define MAX_EMPRESTIMOS 100
#define TAM_TITULO 100
#define TAM_AUTOR 80
#define TAM_EDITORA 60
#define TAM_NOME 100
#define TAM_CURSO 50
#define TAM_TELEFONE 15
#define DIAS_ATRASO 7

// Estrutura para representar datas (dia, mês, ano)
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

// Estrutura para Livro
typedef struct {
    int codigo; // Código do livro (inteiro)
    char titulo[TAM_TITULO];
    char autor[TAM_AUTOR];
    char editora[TAM_EDITORA];
    int ano_publicacao;
    int exemplares_disponiveis;
    char status[15]; // "DISPONIVEL" ou "EMPRESTADO" (apenas para o acervo total)
    int total_exemplares; // Novo campo para rastrear o total
} Livro;

// Estrutura para Usuário
typedef struct {
    int matricula; // Matrícula (inteiro)
    char nome[TAM_NOME];
    char curso[TAM_CURSO];
    char telefone[TAM_TELEFONE];
    Data data_cadastro;
} Usuario;

// Estrutura para Empréstimo
typedef struct {
    int codigo_emprestimo;
    int matricula_usuario;
    int codigo_livro;
    Data data_emprestimo;
    Data data_prevista_devolucao; // 7 dias após empréstimo
    char status[15]; // "ATIVO" ou "DEVOLVIDO"
} Emprestimo;

// Vetores de structs para armazenar os dados
Livro acervo_livros[MAX_LIVROS];
Usuario lista_usuarios[MAX_USUARIOS];
Emprestimo lista_emprestimos[MAX_EMPRESTIMOS];

// Contadores e IDs para o próximo item
int proximo_livro_id = 1;
int proximo_usuario_id = 1;
int proximo_emprestimo_id = 1;
int total_livros = 0;
int total_usuarios = 0;
int total_emprestimos = 0;

// --- FUNÇÕES AUXILIARES GLOBAIS ---

// Limpa o buffer de entrada
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Lê uma string com limite de tamanho, garantindo que não haja estouro
void ler_string(char *str, int tamanho) {
    if (fgets(str, tamanho, stdin) != NULL) {
        // Remove o caractere de nova linha, se estiver presente
        size_t len = strlen(str);
        if (len > 0 && str[len - 1] == '\n') {
            str[len - 1] = '\0';
        } else {
            // Se não encontrou '\n', limpamos o buffer (entrada maior que o limite)
            limpar_buffer();
        }
    }
}

// Verifica se um ano é bissexto
bool eh_bissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

// Retorna o número de dias em um mês
int dias_no_mes(int mes, int ano) {
    if (mes == 2) {
        return eh_bissexto(ano) ? 29 : 28;
    } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        return 30;
    } else {
        return 31;
    }
}

// Retorna a data atual do sistema
Data data_atual() {
    time_t t = time(NULL);
    struct tm *agora = localtime(&t);
    Data d;
    d.dia = agora->tm_mday;
    d.mes = agora->tm_mon + 1;
    d.ano = agora->tm_year + 1900;
    return d;
}

// Calcula a data de devolução (data_base + dias)
Data calcular_data_devolucao(Data data_base, int dias) {
    Data nova_data = data_base;

    for (int i = 0; i < dias; i++) {
        nova_data.dia++;
        int dias_max = dias_no_mes(nova_data.mes, nova_data.ano);

        if (nova_data.dia > dias_max) {
            nova_data.dia = 1;
            nova_data.mes++;
            if (nova_data.mes > 12) {
                nova_data.mes = 1;
                nova_data.ano++;
            }
        }
    }
    return nova_data;
}

// Compara duas datas. Retorna: -1 se d1 < d2, 0 se d1 == d2, 1 se d1 > d2
int comparar_datas(Data d1, Data d2) {
    if (d1.ano != d2.ano) return d1.ano - d2.ano;
    if (d1.mes != d2.mes) return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

// --- PARTE 4: MANIPULAÇÃO DE ARQUIVOS ---

// Caminhos dos arquivos
#define ARQ_LIVROS "livros.txt"
#define ARQ_USUARIOS "usuarios.txt"
#define ARQ_EMPRESTIMOS "emprestimos.txt"

// Função para salvar todos os dados nos arquivos
void salvar_dados() {
    // 1. Salvar Livros
    FILE *f_livros = fopen(ARQ_LIVROS, "w");
    if (f_livros == NULL) {
        printf("\n[ERRO] Nao foi possivel abrir %s para salvar.\n", ARQ_LIVROS);
        return;
    }
    fprintf(f_livros, "%d\n", proximo_livro_id); // Salva o próximo ID
    for (int i = 0; i < total_livros; i++) {
        fprintf(f_livros, "%d;%s;%s;%s;%d;%d;%s;%d\n",
                acervo_livros[i].codigo,
                acervo_livros[i].titulo,
                acervo_livros[i].autor,
                acervo_livros[i].editora,
                acervo_livros[i].ano_publicacao,
                acervo_livros[i].exemplares_disponiveis,
                acervo_livros[i].status,
                acervo_livros[i].total_exemplares);
    }
    fclose(f_livros);

    // 2. Salvar Usuários
    FILE *f_usuarios = fopen(ARQ_USUARIOS, "w");
    if (f_usuarios == NULL) {
        printf("\n[ERRO] Nao foi possivel abrir %s para salvar.\n", ARQ_USUARIOS);
        return;
    }
    fprintf(f_usuarios, "%d\n", proximo_usuario_id); // Salva o próximo ID
    for (int i = 0; i < total_usuarios; i++) {
        fprintf(f_usuarios, "%d;%s;%s;%s;%d/%d/%d\n",
                lista_usuarios[i].matricula,
                lista_usuarios[i].nome,
                lista_usuarios[i].curso,
                lista_usuarios[i].telefone,
                lista_usuarios[i].data_cadastro.dia,
                lista_usuarios[i].data_cadastro.mes,
                lista_usuarios[i].data_cadastro.ano);
    }
    fclose(f_usuarios);

    // 3. Salvar Empréstimos
    FILE *f_emprestimos = fopen(ARQ_EMPRESTIMOS, "w");
    if (f_emprestimos == NULL) {
        printf("\n[ERRO] Nao foi possivel abrir %s para salvar.\n", ARQ_EMPRESTIMOS);
        return;
    }
    fprintf(f_emprestimos, "%d\n", proximo_emprestimo_id); // Salva o próximo ID
    for (int i = 0; i < total_emprestimos; i++) {
        fprintf(f_emprestimos, "%d;%d;%d;%d/%d/%d;%d/%d/%d;%s\n",
                lista_emprestimos[i].codigo_emprestimo,
                lista_emprestimos[i].matricula_usuario,
                lista_emprestimos[i].codigo_livro,
                lista_emprestimos[i].data_emprestimo.dia,
                lista_emprestimos[i].data_emprestimo.mes,
                lista_emprestimos[i].data_emprestimo.ano,
                lista_emprestimos[i].data_prevista_devolucao.dia,
                lista_emprestimos[i].data_prevista_devolucao.mes,
                lista_emprestimos[i].data_prevista_devolucao.ano,
                lista_emprestimos[i].status);
    }
    fclose(f_emprestimos);

    printf("\n[SUCESSO] Dados salvos com sucesso!\n");
}

// Função para carregar dados dos arquivos
void carregar_dados() {
    int id_lido;

    // 1. Carregar Livros
    FILE *f_livros = fopen(ARQ_LIVROS, "r");
    if (f_livros != NULL) {
        if (fscanf(f_livros, "%d\n", &id_lido) == 1) {
            proximo_livro_id = id_lido;
        }

        while (total_livros < MAX_LIVROS &&
               fscanf(f_livros, "%d;%[^;];%[^;];%[^;];%d;%d;%[^;];%d\n",
                      &acervo_livros[total_livros].codigo,
                      acervo_livros[total_livros].titulo,
                      acervo_livros[total_livros].autor,
                      acervo_livros[total_livros].editora,
                      &acervo_livros[total_livros].ano_publicacao,
                      &acervo_livros[total_livros].exemplares_disponiveis,
                      acervo_livros[total_livros].status,
                      &acervo_livros[total_livros].total_exemplares) == 8) {
            total_livros++;
        }
        fclose(f_livros);
        printf("[INFO] %d Livros carregados.\n", total_livros);
    } else {
        printf("[INFO] Arquivo %s nao encontrado. Iniciando com dados vazios.\n", ARQ_LIVROS);
    }

    // 2. Carregar Usuários
    FILE *f_usuarios = fopen(ARQ_USUARIOS, "r");
    if (f_usuarios != NULL) {
        if (fscanf(f_usuarios, "%d\n", &id_lido) == 1) {
            proximo_usuario_id = id_lido;
        }

        while (total_usuarios < MAX_USUARIOS &&
               fscanf(f_usuarios, "%d;%[^;];%[^;];%[^;];%d/%d/%d\n",
                      &lista_usuarios[total_usuarios].matricula,
                      lista_usuarios[total_usuarios].nome,
                      lista_usuarios[total_usuarios].curso,
                      lista_usuarios[total_usuarios].telefone,
                      &lista_usuarios[total_usuarios].data_cadastro.dia,
                      &lista_usuarios[total_usuarios].data_cadastro.mes,
                      &lista_usuarios[total_usuarios].data_cadastro.ano) == 7) {
            total_usuarios++;
        }
        fclose(f_usuarios);
        printf("[INFO] %d Usuarios carregados.\n", total_usuarios);
    } else {
        printf("[INFO] Arquivo %s nao encontrado. Iniciando com dados vazios.\n", ARQ_USUARIOS);
    }

    // 3. Carregar Empréstimos
    FILE *f_emprestimos = fopen(ARQ_EMPRESTIMOS, "r");
    if (f_emprestimos != NULL) {
        if (fscanf(f_emprestimos, "%d\n", &id_lido) == 1) {
            proximo_emprestimo_id = id_lido;
        }

        while (total_emprestimos < MAX_EMPRESTIMOS &&
               fscanf(f_emprestimos, "%d;%d;%d;%d/%d/%d;%d/%d/%d;%s\n",
                      &lista_emprestimos[total_emprestimos].codigo_emprestimo,
                      &lista_emprestimos[total_emprestimos].matricula_usuario,
                      &lista_emprestimos[total_emprestimos].codigo_livro,
                      &lista_emprestimos[total_emprestimos].data_emprestimo.dia,
                      &lista_emprestimos[total_emprestimos].data_emprestimo.mes,
                      &lista_emprestimos[total_emprestimos].data_emprestimo.ano,
                      &lista_emprestimos[total_emprestimos].data_prevista_devolucao.dia,
                      &lista_emprestimos[total_emprestimos].data_prevista_devolucao.mes,
                      &lista_emprestimos[total_emprestimos].data_prevista_devolucao.ano,
                      lista_emprestimos[total_emprestimos].status) == 10) {
            total_emprestimos++;
        }
        fclose(f_emprestimos);
        printf("[INFO] %d Emprestimos carregados.\n", total_emprestimos);
    } else {
        printf("[INFO] Arquivo %s nao encontrado. Iniciando com dados vazios.\n", ARQ_EMPRESTIMOS);
    }
}

// Função para criar backup dos arquivos (cópia simples)
void fazer_backup() {
    char cmd[256];
    char *arquivos[] = {ARQ_LIVROS, ARQ_USUARIOS, ARQ_EMPRESTIMOS};

    printf("\n--- Realizando Backup Automatico ---\n");

    for (int i = 0; i < 3; i++) {
        sprintf(cmd, "cp %s %s.bak", arquivos[i], arquivos[i]); // Comando de cópia Unix/Linux
        // Tenta executar o comando de backup. Em ambientes Windows, isso falhará, mas o requisito é atendido
        if (system(cmd) == 0) {
            printf("[BACKUP] Backup de %s criado em %s.bak\n", arquivos[i], arquivos[i]);
        } else {
            // Em sistemas que não suportam 'cp' (ex: Windows sem cygwin/WSL), tentamos com 'copy' ou avisamos
            sprintf(cmd, "copy %s %s.bak", arquivos[i], arquivos[i]); // Comando de cópia Windows
            if (system(cmd) == 0) {
                printf("[BACKUP] Backup de %s criado em %s.bak (usando copy)\n", arquivos[i], arquivos[i]);
            } else {
                 printf("[AVISO] Nao foi possivel criar backup de %s (falha nos comandos 'cp'/'copy').\n", arquivos[i]);
            }
        }
    }
    printf("--- Backup Concluido ---\n");
}


// --- FUNÇÕES DE BUSCA (Requisito Modular) ---

// Retorna o índice do livro no vetor ou -1 se não encontrado
int buscar_livro_por_codigo(int codigo) {
    for (int i = 0; i < total_livros; i++) {
        if (acervo_livros[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

// Retorna o índice do usuário no vetor ou -1 se não encontrado
int buscar_usuario_por_matricula(int matricula) {
    for (int i = 0; i < total_usuarios; i++) {
        if (lista_usuarios[i].matricula == matricula) {
            return i;
        }
    }
    return -1;
}

// --- PARTE 3: FUNÇÕES MODULARES (CADASTRO) ---

// Função para cadastrar livros
void cadastrar_livro() {
    if (total_livros >= MAX_LIVROS) {
        printf("\n[ERRO] O acervo atingiu o limite maximo de %d livros.\n", MAX_LIVROS);
        return;
    }

    Livro novo_livro;
    novo_livro.codigo = proximo_livro_id++;

    printf("\n--- Cadastro de Novo Livro ---\n");
    printf("Codigo do livro: %d\n", novo_livro.codigo);

    printf("Titulo (max %d): ", TAM_TITULO);
    ler_string(novo_livro.titulo, TAM_TITULO);

    printf("Autor (max %d): ", TAM_AUTOR);
    ler_string(novo_livro.autor, TAM_AUTOR);

    printf("Editora (max %d): ", TAM_EDITORA);
    ler_string(novo_livro.editora, TAM_EDITORA);

    // Validação de entrada: Ano de publicacao
    do {
        printf("Ano de publicacao (ex: 2023): ");
        if (scanf("%d", &novo_livro.ano_publicacao) != 1 || novo_livro.ano_publicacao <= 0) {
            printf("[ERRO] Entrada invalida. Por favor, insira um ano valido.\n");
            limpar_buffer();
        } else {
            break;
        }
    } while (true);

    // Validação de entrada: Total de exemplares
    do {
        printf("Numero TOTAL de exemplares: ");
        if (scanf("%d", &novo_livro.total_exemplares) != 1 || novo_livro.total_exemplares <= 0) {
            printf("[ERRO] Entrada invalida. Por favor, insira um numero positivo de exemplares.\n");
            limpar_buffer();
        } else {
            break;
        }
    } while (true);
    limpar_buffer(); // Limpar buffer após scanf final

    novo_livro.exemplares_disponiveis = novo_livro.total_exemplares;
    strcpy(novo_livro.status, "DISPONIVEL");

    acervo_livros[total_livros++] = novo_livro;
    printf("\n[SUCESSO] Livro '%s' cadastrado com codigo %d.\n", novo_livro.titulo, novo_livro.codigo);
}

// Função para cadastrar usuários
void cadastrar_usuario() {
    if (total_usuarios >= MAX_USUARIOS) {
        printf("\n[ERRO] A lista de usuarios atingiu o limite maximo de %d usuarios.\n", MAX_USUARIOS);
        return;
    }

    Usuario novo_usuario;
    novo_usuario.matricula = proximo_usuario_id++;
    novo_usuario.data_cadastro = data_atual(); // Data de cadastro é a data atual

    printf("\n--- Cadastro de Novo Usuario ---\n");
    printf("Matricula: %d\n", novo_usuario.matricula);

    printf("Nome completo (max %d): ", TAM_NOME);
    ler_string(novo_usuario.nome, TAM_NOME);

    printf("Curso (max %d): ", TAM_CURSO);
    ler_string(novo_usuario.curso, TAM_CURSO);

    printf("Telefone (max %d): ", TAM_TELEFONE);
    ler_string(novo_usuario.telefone, TAM_TELEFONE);

    lista_usuarios[total_usuarios++] = novo_usuario;
    printf("\n[SUCESSO] Usuario '%s' cadastrado com matricula %d em %d/%d/%d.\n",
           novo_usuario.nome, novo_usuario.matricula,
           novo_usuario.data_cadastro.dia, novo_usuario.data_cadastro.mes, novo_usuario.data_cadastro.ano);
}

// --- PARTE 3: FUNÇÕES MODULARES (EMPRÉSTIMOS) ---

// Função para realizar empréstimo
void realizar_emprestimo() {
    if (total_emprestimos >= MAX_EMPRESTIMOS) {
        printf("\n[ERRO] O limite maximo de emprestimos foi atingido.\n");
        return;
    }

    int mat, cod;
    int idx_usuario, idx_livro;

    printf("\n--- Realizar Emprestimo ---\n");

    // Validação de Matrícula
    do {
        printf("Matricula do usuario: ");
        if (scanf("%d", &mat) != 1) {
            printf("[ERRO] Entrada invalida. Digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        idx_usuario = buscar_usuario_por_matricula(mat);
        if (idx_usuario == -1) {
            printf("[ERRO] Usuario com matricula %d nao encontrado.\n", mat);
        } else {
            break;
        }
    } while (true);

    // Validação de Código do Livro
    do {
        printf("Codigo do livro: ");
        if (scanf("%d", &cod) != 1) {
            printf("[ERRO] Entrada invalida. Digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        idx_livro = buscar_livro_por_codigo(cod);
        if (idx_livro == -1) {
            printf("[ERRO] Livro com codigo %d nao encontrado.\n", cod);
        } else if (acervo_livros[idx_livro].exemplares_disponiveis <= 0) {
            printf("[ERRO] Todos os exemplares do livro '%s' estao emprestados.\n", acervo_livros[idx_livro].titulo);
            idx_livro = -1; // Força a nova tentativa ou saída
        } else {
            break;
        }
    } while (idx_livro == -1);

    // Criação do Empréstimo
    Emprestimo novo_emprestimo;
    novo_emprestimo.codigo_emprestimo = proximo_emprestimo_id++;
    novo_emprestimo.matricula_usuario = mat;
    novo_emprestimo.codigo_livro = cod;
    novo_emprestimo.data_emprestimo = data_atual();
    novo_emprestimo.data_prevista_devolucao = calcular_data_devolucao(novo_emprestimo.data_emprestimo, 7);
    strcpy(novo_emprestimo.status, "ATIVO");

    // Atualiza o acervo de livros
    acervo_livros[idx_livro].exemplares_disponiveis--;
    if (acervo_livros[idx_livro].exemplares_disponiveis == 0) {
        strcpy(acervo_livros[idx_livro].status, "INDISPONIVEL");
    } else {
        strcpy(acervo_livros[idx_livro].status, "DISPONIVEL");
    }

    lista_emprestimos[total_emprestimos++] = novo_emprestimo;

    printf("\n[SUCESSO] Emprestimo %d registrado:\n", novo_emprestimo.codigo_emprestimo);
    printf("  Livro: %s\n", acervo_livros[idx_livro].titulo);
    printf("  Usuario: %s\n", lista_usuarios[idx_usuario].nome);
    printf("  Data Emprestimo: %d/%d/%d\n", novo_emprestimo.data_emprestimo.dia, novo_emprestimo.data_emprestimo.mes, novo_emprestimo.data_emprestimo.ano);
    printf("  Data Prevista Devolucao: %d/%d/%d\n", novo_emprestimo.data_prevista_devolucao.dia, novo_emprestimo.data_prevista_devolucao.mes, novo_emprestimo.data_prevista_devolucao.ano);
}

// Função para realizar devolução
void realizar_devolucao() {
    int cod_emp;
    int idx_emprestimo = -1;

    printf("\n--- Realizar Devolucao ---\n");
    printf("Codigo do emprestimo a ser devolvido: ");
    if (scanf("%d", &cod_emp) != 1) {
        printf("[ERRO] Entrada invalida. Digite um numero.\n");
        limpar_buffer();
        return;
    }
    limpar_buffer();

    // Busca o empréstimo ativo
    for (int i = 0; i < total_emprestimos; i++) {
        if (lista_emprestimos[i].codigo_emprestimo == cod_emp && strcmp(lista_emprestimos[i].status, "ATIVO") == 0) {
            idx_emprestimo = i;
            break;
        }
    }

    if (idx_emprestimo == -1) {
        printf("[ERRO] Emprestimo ativo com codigo %d nao encontrado.\n", cod_emp);
        return;
    }

    // Marca como DEVOLVIDO
    strcpy(lista_emprestimos[idx_emprestimo].status, "DEVOLVIDO");

    // Atualiza o acervo de livros
    int idx_livro = buscar_livro_por_codigo(lista_emprestimos[idx_emprestimo].codigo_livro);
    if (idx_livro != -1) {
        acervo_livros[idx_livro].exemplares_disponiveis++;
        if (acervo_livros[idx_livro].exemplares_disponiveis > 0) {
            strcpy(acervo_livros[idx_livro].status, "DISPONIVEL");
        }
    }

    // Verifica Atraso
    Data hoje = data_atual();
    int comparacao = comparar_datas(hoje, lista_emprestimos[idx_emprestimo].data_prevista_devolucao);

    printf("\n[SUCESSO] Devolucao do emprestimo %d registrada.\n", cod_emp);
    if (comparacao > 0) {
        printf("[ATENCAO] Devolucao realizada com atraso!\n");
    } else {
        printf("[INFO] Devolucao realizada no prazo.\n");
    }
}

// Função para renovação de empréstimos (PARTE 5)
void renovar_emprestimo() {
    int cod_emp;
    int idx_emprestimo = -1;

    printf("\n--- Renovar Emprestimo ---\n");
    printf("Codigo do emprestimo a ser renovado: ");
    if (scanf("%d", &cod_emp) != 1) {
        printf("[ERRO] Entrada invalida. Digite um numero.\n");
        limpar_buffer();
        return;
    }
    limpar_buffer();

    // Busca o empréstimo ativo
    for (int i = 0; i < total_emprestimos; i++) {
        if (lista_emprestimos[i].codigo_emprestimo == cod_emp && strcmp(lista_emprestimos[i].status, "ATIVO") == 0) {
            idx_emprestimo = i;
            break;
        }
    }

    if (idx_emprestimo == -1) {
        printf("[ERRO] Emprestimo ativo com codigo %d nao encontrado.\n", cod_emp);
        return;
    }

    // Calcula nova data de devolução a partir da data prevista anterior
    Data nova_data = calcular_data_devolucao(lista_emprestimos[idx_emprestimo].data_prevista_devolucao, 7);

    // Atualiza a data prevista
    lista_emprestimos[idx_emprestimo].data_prevista_devolucao = nova_data;

    printf("\n[SUCESSO] Emprestimo %d renovado por mais 7 dias.\n", cod_emp);
    printf("  Nova Data Prevista Devolucao: %d/%d/%d\n", nova_data.dia, nova_data.mes, nova_data.ano);
}


// --- PARTE 3: FUNÇÕES MODULARES (PESQUISA) ---

// Função para pesquisar livros (por código, título ou autor)
void pesquisar_livros() {
    int opcao;
    printf("\n--- Pesquisar Livros ---\n");
    printf("Buscar por:\n");
    printf("1. Codigo\n");
    printf("2. Titulo\n");
    printf("3. Autor\n");
    printf("4. Busca Avancada (Multiplos Criterios)\n"); // Parte 5
    printf("Opcao: ");
    if (scanf("%d", &opcao) != 1) {
        printf("[ERRO] Opcao invalida.\n");
        limpar_buffer();
        return;
    }
    limpar_buffer();

    Livro *resultados[MAX_LIVROS];
    int num_resultados = 0;

    switch (opcao) {
        case 1: { // Por Código
            int cod;
            printf("Digite o Codigo do livro: ");
            if (scanf("%d", &cod) != 1) {
                printf("[ERRO] Codigo invalido.\n");
                limpar_buffer();
                return;
            }
            limpar_buffer();
            int idx = buscar_livro_por_codigo(cod);
            if (idx != -1) {
                resultados[num_resultados++] = &acervo_livros[idx];
            }
            break;
        }
        case 2: { // Por Título
            char termo[TAM_TITULO];
            printf("Digite o Titulo (ou parte): ");
            ler_string(termo, TAM_TITULO);
            for (int i = 0; i < total_livros; i++) {
                if (strstr(acervo_livros[i].titulo, termo) != NULL) {
                    resultados[num_resultados++] = &acervo_livros[i];
                }
            }
            break;
        }
        case 3: { // Por Autor
            char termo[TAM_AUTOR];
            printf("Digite o Autor (ou parte): ");
            ler_string(termo, TAM_AUTOR);
            for (int i = 0; i < total_livros; i++) {
                if (strstr(acervo_livros[i].autor, termo) != NULL) {
                    resultados[num_resultados++] = &acervo_livros[i];
                }
            }
            break;
        }
        case 4: { // Busca Avançada (Parte 5)
            char titulo[TAM_TITULO] = "";
            char autor[TAM_AUTOR] = "";
            int ano = 0;

            printf("\n--- Busca Avancada (Deixe em branco/0 para ignorar) ---\n");
            printf("Titulo (ou parte): ");
            ler_string(titulo, TAM_TITULO);

            printf("Autor (ou parte): ");
            ler_string(autor, TAM_AUTOR);

            printf("Ano de Publicacao (0 para ignorar): ");
            if (scanf("%d", &ano) != 1) {
                limpar_buffer();
            }
            limpar_buffer();

            for (int i = 0; i < total_livros; i++) {
                bool match_titulo = (strlen(titulo) == 0 || strstr(acervo_livros[i].titulo, titulo) != NULL);
                bool match_autor = (strlen(autor) == 0 || strstr(acervo_livros[i].autor, autor) != NULL);
                bool match_ano = (ano == 0 || acervo_livros[i].ano_publicacao == ano);

                if (match_titulo && match_autor && match_ano) {
                    resultados[num_resultados++] = &acervo_livros[i];
                }
            }
            break;
        }
        default:
            printf("[ERRO] Opcao invalida.\n");
            return;
    }

    // Exibição dos resultados
    if (num_resultados > 0) {
        printf("\n--- Resultados da Pesquisa (%d encontrado(s)) ---\n", num_resultados);
        for (int i = 0; i < num_resultados; i++) {
            printf("------------------------------------------\n");
            printf("Codigo: %d\n", resultados[i]->codigo);
            printf("Titulo: %s\n", resultados[i]->titulo);
            printf("Autor: %s\n", resultados[i]->autor);
            printf("Editora: %s\n", resultados[i]->editora);
            printf("Ano: %d\n", resultados[i]->ano_publicacao);
            printf("Total Exemplares: %d\n", resultados[i]->total_exemplares);
            printf("Disponiveis: %d\n", resultados[i]->exemplares_disponiveis);
            printf("Status: %s\n", resultados[i]->status);
        }
        printf("------------------------------------------\n");
    } else {
        printf("\n[INFO] Nenhum livro encontrado com os criterios fornecidos.\n");
    }
}

// Função para pesquisar usuários (por matrícula ou nome)
void pesquisar_usuarios() {
    int opcao;
    printf("\n--- Pesquisar Usuarios ---\n");
    printf("Buscar por:\n");
    printf("1. Matricula\n");
    printf("2. Nome\n");
    printf("Opcao: ");
    if (scanf("%d", &opcao) != 1) {
        printf("[ERRO] Opcao invalida.\n");
        limpar_buffer();
        return;
    }
    limpar_buffer();

    Usuario *resultados[MAX_USUARIOS];
    int num_resultados = 0;

    switch (opcao) {
        case 1: { // Por Matrícula
            int mat;
            printf("Digite a Matricula do usuario: ");
            if (scanf("%d", &mat) != 1) {
                printf("[ERRO] Matricula invalida.\n");
                limpar_buffer();
                return;
            }
            limpar_buffer();
            int idx = buscar_usuario_por_matricula(mat);
            if (idx != -1) {
                resultados[num_resultados++] = &lista_usuarios[idx];
            }
            break;
        }
        case 2: { // Por Nome
            char termo[TAM_NOME];
            printf("Digite o Nome completo (ou parte): ");
            ler_string(termo, TAM_NOME);
            for (int i = 0; i < total_usuarios; i++) {
                if (strstr(lista_usuarios[i].nome, termo) != NULL) {
                    resultados[num_resultados++] = &lista_usuarios[i];
                }
            }
            break;
        }
        default:
            printf("[ERRO] Opcao invalida.\n");
            return;
    }

    // Exibição dos resultados
    if (num_resultados > 0) {
        printf("\n--- Resultados da Pesquisa (%d encontrado(s)) ---\n", num_resultados);
        for (int i = 0; i < num_resultados; i++) {
            printf("------------------------------------------\n");
            printf("Matricula: %d\n", resultados[i]->matricula);
            printf("Nome: %s\n", resultados[i]->nome);
            printf("Curso: %s\n", resultados[i]->curso);
            printf("Telefone: %s\n", resultados[i]->telefone);
            printf("Data Cadastro: %d/%d/%d\n", resultados[i]->data_cadastro.dia, resultados[i]->data_cadastro.mes, resultados[i]->data_cadastro.ano);
        }
        printf("------------------------------------------\n");
    } else {
        printf("\n[INFO] Nenhum usuario encontrado com os criterios fornecidos.\n");
    }
}

// Função para listar empréstimos ativos
void listar_emprestimos_ativos() {
    int contador = 0;
    printf("\n--- Lista de Emprestimos Ativos ---\n");
    printf("Data Atual: %d/%d/%d\n", data_atual().dia, data_atual().mes, data_atual().ano);

    printf("Cod. Emp | Matr. Usuario | Cod. Livro | Data Emp. | Data Prev. Dev. | Status\n");
    printf("---------------------------------------------------------------------------\n");

    for (int i = 0; i < total_emprestimos; i++) {
        if (strcmp(lista_emprestimos[i].status, "ATIVO") == 0) {
            printf("%8d | %13d | %10d | %02d/%02d/%04d | %02d/%02d/%04d | %s\n",
                   lista_emprestimos[i].codigo_emprestimo,
                   lista_emprestimos[i].matricula_usuario,
                   lista_emprestimos[i].codigo_livro,
                   lista_emprestimos[i].data_emprestimo.dia,
                   lista_emprestimos[i].data_emprestimo.mes,
                   lista_emprestimos[i].data_emprestimo.ano,
                   lista_emprestimos[i].data_prevista_devolucao.dia,
                   lista_emprestimos[i].data_prevista_devolucao.mes,
                   lista_emprestimos[i].data_prevista_devolucao.ano,
                   lista_emprestimos[i].status);
            contador++;
        }
    }

    printf("---------------------------------------------------------------------------\n");
    printf("Total de emprestimos ativos: %d\n", contador);

    if (contador == 0) {
        printf("[INFO] Nao ha emprestimos ativos no momento.\n");
    }
}

// --- PARTE 5: FUNCIONALIDADES AVANÇADAS (RELATÓRIOS) ---

// Relatório de livros mais emprestados
void relatorio_livros_mais_emprestados() {
    printf("\n--- Relatorio de Livros Mais Emprestados ---\n");

    if (total_emprestimos == 0) {
        printf("[INFO] Nao ha emprestimos registrados para gerar o relatorio.\n");
        return;
    }

    int contagem_emprestimos[MAX_LIVROS] = {0};
    int livro_codigos[MAX_LIVROS];
    int num_livros_distintos = 0;

    // 1. Contar as ocorrências de cada livro em todos os empréstimos (ativos e devolvidos)
    for (int i = 0; i < total_emprestimos; i++) {
        int cod = lista_emprestimos[i].codigo_livro;
        int encontrado = 0;
        for (int j = 0; j < num_livros_distintos; j++) {
            if (livro_codigos[j] == cod) {
                contagem_emprestimos[j]++;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado && num_livros_distintos < MAX_LIVROS) {
            livro_codigos[num_livros_distintos] = cod;
            contagem_emprestimos[num_livros_distintos]++;
            num_livros_distintos++;
        }
    }

    // 2. Ordenar os livros por contagem (Bubble Sort simples)
    for (int i = 0; i < num_livros_distintos - 1; i++) {
        for (int j = 0; j < num_livros_distintos - i - 1; j++) {
            if (contagem_emprestimos[j] < contagem_emprestimos[j + 1]) {
                // Troca a contagem
                int temp_count = contagem_emprestimos[j];
                contagem_emprestimos[j] = contagem_emprestimos[j + 1];
                contagem_emprestimos[j + 1] = temp_count;
                // Troca o código
                int temp_cod = livro_codigos[j];
                livro_codigos[j] = livro_codigos[j + 1];
                livro_codigos[j + 1] = temp_cod;
            }
        }
    }

    // 3. Exibir o resultado
    printf("RANK | Codigo | Titulo | Total Emprestimos\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < num_livros_distintos; i++) {
        int idx_livro = buscar_livro_por_codigo(livro_codigos[i]);
        if (idx_livro != -1) {
            printf("%4d | %6d | %-10s | %17d\n",
                   i + 1,
                   livro_codigos[i],
                   acervo_livros[idx_livro].titulo,
                   contagem_emprestimos[i]);
        }
    }
    printf("--------------------------------------------\n");
}

// Relatório de usuários com empréstimos em atraso
void relatorio_usuarios_em_atraso() {
    printf("\n--- Relatorio de Usuarios com Emprestimos em Atraso ---\n");
    Data hoje = data_atual();
    int contador = 0;

    printf("Data Atual: %d/%d/%d\n", hoje.dia, hoje.mes, hoje.ano);
    printf("Matricula | Nome do Usuario | Cod. Emp | Data Prev. Dev.\n");
    printf("-------------------------------------------------------------------\n");

    for (int i = 0; i < total_emprestimos; i++) {
        // Verifica se está ATIVO e se HOJE é depois da DATA PREVISTA
        if (strcmp(lista_emprestimos[i].status, "ATIVO") == 0 &&
            comparar_datas(hoje, lista_emprestimos[i].data_prevista_devolucao) > 0) {

            int idx_usuario = buscar_usuario_por_matricula(lista_emprestimos[i].matricula_usuario);

            if (idx_usuario != -1) {
                printf("%9d | %-15s | %8d | %02d/%02d/%04d\n",
                       lista_emprestimos[i].matricula_usuario,
                       lista_usuarios[idx_usuario].nome,
                       lista_emprestimos[i].codigo_emprestimo,
                       lista_emprestimos[i].data_prevista_devolucao.dia,
                       lista_emprestimos[i].data_prevista_devolucao.mes,
                       lista_emprestimos[i].data_prevista_devolucao.ano);
                contador++;
            }
        }
    }

    printf("-------------------------------------------------------------------\n");
    printf("Total de emprestimos em atraso: %d\n", contador);

    if (contador == 0) {
        printf("[INFO] Parabens! Nenhum emprestimo em atraso encontrado.\n");
    }
}


// --- PARTE 2: SISTEMA DE MENUS E CONTROLE DE FLUXO ---

void menu_livros() {
    int opcao;
    do {
        printf("\n========== Menu Livros ==========\n");
        printf("1. Cadastrar Novo Livro\n");
        printf("2. Pesquisar Livro\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("[ERRO] Entrada invalida. Por favor, digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                cadastrar_livro();
                break;
            case 2:
                pesquisar_livros();
                break;
            case 0:
                printf("[INFO] Voltando ao Menu Principal.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

void menu_usuarios() {
    int opcao;
    do {
        printf("\n========== Menu Usuarios ==========\n");
        printf("1. Cadastrar Novo Usuario\n");
        printf("2. Pesquisar Usuario\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("[ERRO] Entrada invalida. Por favor, digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                cadastrar_usuario();
                break;
            case 2:
                pesquisar_usuarios();
                break;
            case 0:
                printf("[INFO] Voltando ao Menu Principal.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

void menu_emprestimos() {
    int opcao;
    do {
        printf("\n========== Menu Emprestimos ==========\n");
        printf("1. Realizar Emprestimo\n");
        printf("2. Realizar Devolucao\n");
        printf("3. Renovar Emprestimo\n"); // Parte 5
        printf("4. Listar Emprestimos Ativos\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("[ERRO] Entrada invalida. Por favor, digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                realizar_emprestimo();
                break;
            case 2:
                realizar_devolucao();
                break;
            case 3:
                renovar_emprestimo();
                break;
            case 4:
                listar_emprestimos_ativos();
                break;
            case 0:
                printf("[INFO] Voltando ao Menu Principal.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

void menu_relatorios() {
    int opcao;
    do {
        printf("\n========== Menu Relatorios (Avancados) ==========\n");
        printf("1. Livros Mais Emprestados\n");
        printf("2. Usuarios com Emprestimos em Atraso\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("[ERRO] Entrada invalida. Por favor, digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                relatorio_livros_mais_emprestados();
                break;
            case 2:
                relatorio_usuarios_em_atraso();
                break;
            case 0:
                printf("[INFO] Voltando ao Menu Principal.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

void menu_principal() {
    int opcao;
    do {
        printf("\n============================================\n");
        printf("  SISTEMA DE GERENCIAMENTO DE BIBLIOTECA\n");
        printf("============================================\n");
        printf("1. Gerenciar Livros\n");
        printf("2. Gerenciar Usuarios\n");
        printf("3. Gerenciar Emprestimos e Devolucoes\n");
        printf("4. Relatorios Avancados\n");
        printf("5. Realizar Backup Manual dos Dados\n");
        printf("0. Sair do Sistema (Salvar e Fechar)\n");
        printf("--------------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("[ERRO] Entrada invalida. Por favor, digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                menu_livros();
                break;
            case 2:
                menu_usuarios();
                break;
            case 3:
                menu_emprestimos();
                break;
            case 4:
                menu_relatorios();
                break;
            case 5:
                fazer_backup();
                break;
            case 0:
                printf("\nEncerrando o sistema...\n");
                break;
            default:
                printf("[ERRO] Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

// --- FUNÇÃO PRINCIPAL ---

int main() {
    printf("Iniciando Sistema de Gerenciamento de Biblioteca...\n");

    // Parte 4: Carregar dados na inicialização
    carregar_dados();

    // Parte 2: Menu Principal
    menu_principal();

    // Parte 4: Salvar dados no encerramento
    salvar_dados();

    printf("\nSistema encerrado. Obrigado!\n");

    return 0;
}
