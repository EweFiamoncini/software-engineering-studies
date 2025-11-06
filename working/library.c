#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- PARTE 1: ESTRUTURAS DE DADOS E CONSTANTES ---

// Constantes para o tamanho máximo dos vetores de structs
#define MAX_LIVROS 100
#define MAX_USUARIOS 50
#define MAX_EMPRESTIMOS 200

// Estrutura para representar datas
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

// Estrutura para representar um Livro
typedef struct {
    int codigo;
    char titulo[101];
    char autor[81];
    char editora[61];
    int ano_publicacao;
    int exemplares;         // Total de exemplares
    int emprestados;        // Exemplares atualmente emprestados
    int vezes_emprestado;   // Para o relatório de mais emprestados
    // O status (disponível/emprestado) é inferido a partir de 'exemplares' e 'emprestados'
} Livro;

// Estrutura para representar um Usuário
typedef struct {
    int matricula;
    char nome[101];
    char curso[51];
    char telefone[16];
    Data data_cadastro;
} Usuario;

// Estrutura para representar um Empréstimo
typedef struct {
    int codigo_emprestimo;
    int matricula_usuario;
    int codigo_livro;
    Data data_emprestimo;
    Data data_devolucao_prevista;
    char status[15]; // "Ativo" ou "Devolvido"
} Emprestimo;

// Vetores de structs para armazenar os dados (Acervo)
Livro livros[MAX_LIVROS];
Usuario usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

// Contadores de elementos
int cont_livros = 0;
int cont_usuarios = 0;
int cont_emprestimos = 0;

// Variáveis globais auxiliares
int proximo_cod_livro = 1;
int proxima_matricula = 1;
int proximo_cod_emprestimo = 1;

// --- FUNÇÕES AUXILIARES DE DATA E VALIDAÇÃO ---

/**
 * @brief Verifica se um ano é bissexto.
 */
int eh_bissexto(int ano) {
    return (ano % 400 == 0) || ( (ano % 4 == 0) && (ano % 100 != 0) );
}

/**
 * @brief Retorna o número de dias em um determinado mês/ano.
 */
int dias_no_mes(int mes, int ano) {
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) return 30;
    if (mes == 2) return eh_bissexto(ano) ? 29 : 28;
    return 31;
}

/**
 * @brief Valida se uma data é logicamente possível.
 */
int validar_data(int d, int m, int a) {
    if (a < 2000 || a > 2100) return 0; // Ano razoável
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > dias_no_mes(m, a)) return 0;
    return 1;
}

/**
 * @brief Adiciona 7 dias a uma data e calcula a data de devolução prevista.
 * @param data_emprestimo A data de início (passada por valor).
 * @return A data 7 dias após o empréstimo.
 */
Data calcular_data_devolucao(Data data_emprestimo) {
    Data data_devolucao = data_emprestimo;
    int dias_a_adicionar = 7;

    while (dias_a_adicionar > 0) {
        int dias_restantes_no_mes = dias_no_mes(data_devolucao.mes, data_devolucao.ano) - data_devolucao.dia;

        if (dias_restantes_no_mes >= dias_a_adicionar) {
            data_devolucao.dia += dias_a_adicionar;
            dias_a_adicionar = 0;
        } else {
            dias_a_adicionar -= (dias_restantes_no_mes + 1); // +1 porque vai para o próximo mês
            data_devolucao.dia = 1;
            data_devolucao.mes++;
            if (data_devolucao.mes > 12) {
                data_devolucao.mes = 1;
                data_devolucao.ano++;
            }
        }
    }
    return data_devolucao;
}

/**
 * @brief Compara duas datas. Retorna > 0 se data1 for mais recente, < 0 se data2 for mais recente, 0 se forem iguais.
 * Para o relatório de atraso.
 */
int comparar_datas(Data d1, Data d2) {
    if (d1.ano != d2.ano) return d1.ano - d2.ano;
    if (d1.mes != d2.mes) return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

/**
 * @brief Retorna a data atual do sistema para verificar atrasos.
 * NOTE: Para simulação, vou usar uma data fixa ou pedir ao usuário,
 * mas para um sistema funcional, a data real seria ideal.
 * Aqui, vamos simular a data atual lendo do sistema ou do usuário.
 */
Data obter_data_atual_simulada() {
    Data hoje;
    printf("\n[SIMULACAO] Digite a data de HOJE (dd mm aaaa): ");
    while (scanf("%d %d %d", &hoje.dia, &hoje.mes, &hoje.ano) != 3 || !validar_data(hoje.dia, hoje.mes, hoje.ano)) {
        printf("Data invalida. Tente novamente (dd mm aaaa): ");
        // Nenhuma limpeza de buffer
    }
    // Nenhuma limpeza de buffer
    return hoje;
}

// --- FUNÇÕES DE MANIPULAÇÃO DE ARQUIVOS (PARTE 4) ---

/**
 * @brief Salva todos os dados (Livros, Usuários, Empréstimos) em seus respectivos arquivos.
 * (Implementa o backup automático dos dados)
 */
void salvar_dados() {
    FILE *f_livros = fopen("livros.txt", "w");
    FILE *f_usuarios = fopen("usuarios.txt", "w");
    FILE *f_emprestimos = fopen("emprestimos.txt", "w");

    if (!f_livros || !f_usuarios || !f_emprestimos) {
        printf("\nERRO: Nao foi possivel abrir todos os arquivos para salvar.\n");
        return;
    }

    // 1. Salvar Livros
    for (int i = 0; i < cont_livros; i++) {
        fprintf(f_livros, "%d;%s;%s;%s;%d;%d;%d;%d\n",
            livros[i].codigo,
            livros[i].titulo,
            livros[i].autor,
            livros[i].editora,
            livros[i].ano_publicacao,
            livros[i].exemplares,
            livros[i].emprestados,
            livros[i].vezes_emprestado);
    }

    // 2. Salvar Usuários
    for (int i = 0; i < cont_usuarios; i++) {
        fprintf(f_usuarios, "%d;%s;%s;%s;%d-%d-%d\n",
            usuarios[i].matricula,
            usuarios[i].nome,
            usuarios[i].curso,
            usuarios[i].telefone,
            usuarios[i].data_cadastro.dia,
            usuarios[i].data_cadastro.mes,
            usuarios[i].data_cadastro.ano);
    }

    // 3. Salvar Empréstimos
    for (int i = 0; i < cont_emprestimos; i++) {
        fprintf(f_emprestimos, "%d;%d;%d;%d-%d-%d;%d-%d-%d;%s\n",
            emprestimos[i].codigo_emprestimo,
            emprestimos[i].matricula_usuario,
            emprestimos[i].codigo_livro,
            emprestimos[i].data_emprestimo.dia,
            emprestimos[i].data_emprestimo.mes,
            emprestimos[i].data_emprestimo.ano,
            emprestimos[i].data_devolucao_prevista.dia,
            emprestimos[i].data_devolucao_prevista.mes,
            emprestimos[i].data_devolucao_prevista.ano,
            emprestimos[i].status);
    }

    fclose(f_livros);
    fclose(f_usuarios);
    fclose(f_emprestimos);

    printf("\n[Sistema] Dados salvos com sucesso (backup automatico).\n");
}

/**
 * @brief Carrega todos os dados dos arquivos na inicialização do programa.
 */
void carregar_dados() {
    FILE *f_livros = fopen("livros.txt", "r");
    FILE *f_usuarios = fopen("usuarios.txt", "r");
    FILE *f_emprestimos = fopen("emprestimos.txt", "r");

    int max_cod_livro = 0;
    int max_matricula = 0;
    int max_cod_emprestimo = 0;

    // 1. Carregar Livros
    if (f_livros) {
        Livro l;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), f_livros)) {
            if (sscanf(buffer, "%d;%100[^;];%80[^;];%60[^;];%d;%d;%d;%d",
                       &l.codigo, l.titulo, l.autor, l.editora, &l.ano_publicacao,
                       &l.exemplares, &l.emprestados, &l.vezes_emprestado) == 8) {
                if (cont_livros < MAX_LIVROS) {
                    livros[cont_livros++] = l;
                    if (l.codigo > max_cod_livro) max_cod_livro = l.codigo;
                }
            }
        }
        fclose(f_livros);
        printf("[Sistema] %d livros carregados.\n", cont_livros);
    }

    // 2. Carregar Usuários
    if (f_usuarios) {
        Usuario u;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), f_usuarios)) {
            if (sscanf(buffer, "%d;%100[^;];%50[^;];%15[^;];%d-%d-%d",
                       &u.matricula, u.nome, u.curso, u.telefone,
                       &u.data_cadastro.dia, &u.data_cadastro.mes, &u.data_cadastro.ano) == 7) {
                if (cont_usuarios < MAX_USUARIOS) {
                    usuarios[cont_usuarios++] = u;
                    if (u.matricula > max_matricula) max_matricula = u.matricula;
                }
            }
        }
        fclose(f_usuarios);
        printf("[Sistema] %d usuarios carregados.\n", cont_usuarios);
    }

    // 3. Carregar Empréstimos
    if (f_emprestimos) {
        Emprestimo e;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), f_emprestimos)) {
            if (sscanf(buffer, "%d;%d;%d;%d-%d-%d;%d-%d-%d;%14s",
                       &e.codigo_emprestimo, &e.matricula_usuario, &e.codigo_livro,
                       &e.data_emprestimo.dia, &e.data_emprestimo.mes, &e.data_emprestimo.ano,
                       &e.data_devolucao_prevista.dia, &e.data_devolucao_prevista.mes, &e.data_devolucao_prevista.ano,
                       e.status) == 10) {
                if (cont_emprestimos < MAX_EMPRESTIMOS) {
                    emprestimos[cont_emprestimos++] = e;
                    if (e.codigo_emprestimo > max_cod_emprestimo) max_cod_emprestimo = e.codigo_emprestimo;
                }
            }
        }
        fclose(f_emprestimos);
        printf("[Sistema] %d emprestimos carregados.\n", cont_emprestimos);
    }

    // Atualiza os próximos códigos/matrículas
    proximo_cod_livro = max_cod_livro + 1;
    proxima_matricula = max_matricula + 1;
    proximo_cod_emprestimo = max_cod_emprestimo + 1;

    printf("[Sistema] Inicializacao de estruturas concluida.\n");
}

// --- FUNÇÕES DE BUSCA (AUXILIARES PARA OUTRAS FUNÇÕES) ---

/**
 * @brief Busca um livro pelo código e retorna seu índice no vetor.
 * @param codigo O código a ser buscado.
 * @return O índice do livro ou -1 se não encontrado.
 */
int buscar_livro_por_codigo(int codigo) {
    for (int i = 0; i < cont_livros; i++) {
        if (livros[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Busca um usuário pela matrícula e retorna seu índice no vetor.
 * @param matricula A matrícula a ser buscada.
 * @return O índice do usuário ou -1 se não encontrado.
 */
int buscar_usuario_por_matricula(int matricula) {
    for (int i = 0; i < cont_usuarios; i++) {
        if (usuarios[i].matricula == matricula) {
            return i;
        }
    }
    return -1;
}

// --- FUNÇÕES MODULARES (PARTE 3) ---

// --- 1. FUNÇÕES DE CADASTRO ---

/**
 * @brief Cadastra um novo livro no sistema.
 */
void cadastrar_livro() {
    if (cont_livros >= MAX_LIVROS) {
        printf("\nERRO: Limite maximo de livros atingido (%d).\n", MAX_LIVROS);
        return;
    }

    Livro novo_livro;

    novo_livro.codigo = proximo_cod_livro++;
    printf("\n--- CADASTRO DE LIVRO ---\n");
    printf("Codigo do livro (Automatico): %d\n", novo_livro.codigo);

    // Título
    printf("Titulo (max 100 caracteres): ");
    // O usuário deve garantir que o buffer esteja limpo antes de chamar fgets
    fgets(novo_livro.titulo, 101, stdin);
    novo_livro.titulo[strcspn(novo_livro.titulo, "\n")] = 0; // Remove newline

    // Autor
    printf("Autor (max 80 caracteres): ");
    fgets(novo_livro.autor, 81, stdin);
    novo_livro.autor[strcspn(novo_livro.autor, "\n")] = 0;

    // Editora
    printf("Editora (max 60 caracteres): ");
    fgets(novo_livro.editora, 61, stdin);
    novo_livro.editora[strcspn(novo_livro.editora, "\n")] = 0;

    // Ano de Publicação (Validação de entrada)
    printf("Ano de publicacao (yyyy): ");
    while (scanf("%d", &novo_livro.ano_publicacao) != 1 || novo_livro.ano_publicacao < 1500 || novo_livro.ano_publicacao > 2050) {
        printf("Ano invalido. Digite um ano entre 1500 e 2050: ");
        // Nenhuma limpeza de buffer
    }

    // Número de Exemplares (Validação de entrada)
    printf("Numero de exemplares disponiveis: ");
    while (scanf("%d", &novo_livro.exemplares) != 1 || novo_livro.exemplares <= 0) {
        printf("Numero de exemplares invalido. Digite um valor positivo: ");
        // Nenhuma limpeza de buffer
    }
    // Nenhuma limpeza de buffer

    // Inicializa campos de controle
    novo_livro.emprestados = 0;
    novo_livro.vezes_emprestado = 0;

    livros[cont_livros++] = novo_livro;
    printf("\nLivro '%s' cadastrado com sucesso! Codigo: %d\n", novo_livro.titulo, novo_livro.codigo);
    salvar_dados();
}

/**
 * @brief Cadastra um novo usuário no sistema.
 */
void cadastrar_usuario() {
    if (cont_usuarios >= MAX_USUARIOS) {
        printf("\nERRO: Limite maximo de usuarios atingido (%d).\n", MAX_USUARIOS);
        return;
    }

    Usuario novo_usuario;

    novo_usuario.matricula = proxima_matricula++;
    printf("\n--- CADASTRO DE USUARIO ---\n");
    printf("Matricula (Automatico): %d\n", novo_usuario.matricula);

    // Nome Completo
    printf("Nome completo (max 100 caracteres): ");
    // O usuário deve garantir que o buffer esteja limpo antes de chamar fgets
    fgets(novo_usuario.nome, 101, stdin);
    novo_usuario.nome[strcspn(novo_usuario.nome, "\n")] = 0;

    // Curso
    printf("Curso (max 50 caracteres): ");
    fgets(novo_usuario.curso, 51, stdin);
    novo_usuario.curso[strcspn(novo_usuario.curso, "\n")] = 0;

    // Telefone
    printf("Telefone (max 15 caracteres): ");
    fgets(novo_usuario.telefone, 16, stdin);
    novo_usuario.telefone[strcspn(novo_usuario.telefone, "\n")] = 0;

    // Data de Cadastro
    printf("Data de cadastro (dd mm aaaa): ");
    while (scanf("%d %d %d", &novo_usuario.data_cadastro.dia, &novo_usuario.data_cadastro.mes, &novo_usuario.data_cadastro.ano) != 3 ||
           !validar_data(novo_usuario.data_cadastro.dia, novo_usuario.data_cadastro.mes, novo_usuario.data_cadastro.ano)) {
        printf("Data invalida. Tente novamente (dd mm aaaa): ");
        // Nenhuma limpeza de buffer
    }
    // Nenhuma limpeza de buffer

    usuarios[cont_usuarios++] = novo_usuario;
    printf("\nUsuario '%s' cadastrado com sucesso! Matricula: %d\n", novo_usuario.nome, novo_usuario.matricula);
    salvar_dados();
}

// --- 2. FUNÇÕES DE EMPRÉSTIMO ---

/**
 * @brief Realiza o empréstimo de um livro para um usuário.
 */
void realizar_emprestimo() {
    int mat, cod_liv;
    int idx_usuario, idx_livro;

    if (cont_emprestimos >= MAX_EMPRESTIMOS) {
        printf("\nERRO: Limite maximo de emprestimos atingido (%d).\n", MAX_EMPRESTIMOS);
        return;
    }

    printf("\n--- NOVO EMPRESTIMO ---\n");

    // 1. Validar Usuário
    printf("Digite a matricula do usuario: ");
    while (scanf("%d", &mat) != 1) {
        printf("Entrada invalida. Digite a matricula do usuario: ");
        // Nenhuma limpeza de buffer
    }
    // Nenhuma limpeza de buffer
    idx_usuario = buscar_usuario_por_matricula(mat);

    if (idx_usuario == -1) {
        printf("ERRO: Usuario com matricula %d nao encontrado.\n", mat);
        return;
    }

    // 2. Validar Livro e Disponibilidade
    printf("Digite o codigo do livro: ");
    while (scanf("%d", &cod_liv) != 1) {
        printf("Entrada invalida. Digite o codigo do livro: ");
        // Nenhuma limpeza de buffer
    }
    // Nenhuma limpeza de buffer
    idx_livro = buscar_livro_por_codigo(cod_liv);

    if (idx_livro == -1) {
        printf("ERRO: Livro com codigo %d nao encontrado.\n", cod_liv);
        return;
    }

    if (livros[idx_livro].exemplares <= livros[idx_livro].emprestados) {
        printf("ERRO: Todos os exemplares do livro '%s' estao emprestados.\n", livros[idx_livro].titulo);
        return;
    }

    // 3. Registrar Empréstimo
    Emprestimo novo_emprestimo;
    novo_emprestimo.codigo_emprestimo = proximo_cod_emprestimo++;
    novo_emprestimo.matricula_usuario = mat;
    novo_emprestimo.codigo_livro = cod_liv;

    // Data do Empréstimo
    printf("Digite a data do emprestimo (dd mm aaaa): ");
    while (scanf("%d %d %d", &novo_emprestimo.data_emprestimo.dia, &novo_emprestimo.data_emprestimo.mes, &novo_emprestimo.data_emprestimo.ano) != 3 ||
           !validar_data(novo_emprestimo.data_emprestimo.dia, novo_emprestimo.data_emprestimo.mes, novo_emprestimo.data_emprestimo.ano)) {
        printf("Data invalida. Tente novamente (dd mm aaaa): ");
        // Nenhuma limpeza de buffer
    }
    // Nenhuma limpeza de buffer

    // Data Prevista de Devolução (7 dias após empréstimo)
    novo_emprestimo.data_devolucao_prevista = calcular_data_devolucao(novo_emprestimo.data_emprestimo);

    strcpy(novo_emprestimo.status, "Ativo");

    // Atualiza o Livro
    livros[idx_livro].emprestados++;
    livros[idx_livro].vezes_emprestado++;

    // Salva o Empréstimo
    emprestimos[cont_emprestimos++] = novo_emprestimo;

    printf("\nEmprestimo registrado com sucesso! Codigo: %d\n", novo_emprestimo.codigo_emprestimo);
    printf("Devolucao prevista para: %02d/%02d/%d\n",
           novo_emprestimo.data_devolucao_prevista.dia,
           novo_emprestimo.data_devolucao_prevista.mes,
           novo_emprestimo.data_devolucao_prevista.ano);

    salvar_dados();
}

/**
 * @brief Realiza a devolução de um livro.
 */
void realizar_devolucao() {
    int cod_emp;
    printf("\n--- DEVOLUCAO DE LIVRO ---\n");
    printf("Digite o codigo do emprestimo a ser finalizado: ");

    if (scanf("%d", &cod_emp) != 1) {
        printf("Entrada invalida.\n");
        // Nenhuma limpeza de buffer
        return;
    }
    // Nenhuma limpeza de buffer

    int idx_emprestimo = -1;
    for (int i = 0; i < cont_emprestimos; i++) {
        if (emprestimos[i].codigo_emprestimo == cod_emp && strcmp(emprestimos[i].status, "Ativo") == 0) {
            idx_emprestimo = i;
            break;
        }
    }

    if (idx_emprestimo == -1) {
        printf("ERRO: Emprestimo ativo com codigo %d nao encontrado.\n", cod_emp);
        return;
    }

    int idx_livro = buscar_livro_por_codigo(emprestimos[idx_emprestimo].codigo_livro);

    // 1. Atualiza Status do Empréstimo
    strcpy(emprestimos[idx_emprestimo].status, "Devolvido");

    // 2. Atualiza Livro (diminui o contador de emprestados)
    if (idx_livro != -1) {
        livros[idx_livro].emprestados--;
    } else {
        printf("AVISO: Livro associado nao encontrado (possivel inconsistencia).\n");
    }

    printf("\nDevolucao do emprestimo %d registrada com sucesso.\n", cod_emp);
    salvar_dados();
}

/**
 * @brief Permite a renovação de um empréstimo ativo.
 */
void renovar_emprestimo() {
    int cod_emp;
    printf("\n--- RENOVACAO DE EMPRESTIMO ---\n");
    printf("Digite o codigo do emprestimo a ser renovado: ");

    if (scanf("%d", &cod_emp) != 1) {
        printf("Entrada invalida.\n");
        // Nenhuma limpeza de buffer
        return;
    }
    // Nenhuma limpeza de buffer

    int idx_emprestimo = -1;
    for (int i = 0; i < cont_emprestimos; i++) {
        if (emprestimos[i].codigo_emprestimo == cod_emp && strcmp(emprestimos[i].status, "Ativo") == 0) {
            idx_emprestimo = i;
            break;
        }
    }

    if (idx_emprestimo == -1) {
        printf("ERRO: Emprestimo ativo com codigo %d nao encontrado.\n", cod_emp);
        return;
    }

    // Calcula a nova data de devolução (7 dias a partir da data prevista anterior)
    emprestimos[idx_emprestimo].data_devolucao_prevista = calcular_data_devolucao(emprestimos[idx_emprestimo].data_devolucao_prevista);

    printf("\nEmprestimo %d RENOVADO com sucesso!\n", cod_emp);
    printf("Nova devolucao prevista para: %02d/%02d/%d\n",
           emprestimos[idx_emprestimo].data_devolucao_prevista.dia,
           emprestimos[idx_emprestimo].data_devolucao_prevista.mes,
           emprestimos[idx_emprestimo].data_devolucao_prevista.ano);

    salvar_dados();
}

// --- 3. FUNÇÕES DE PESQUISA E LISTAGEM ---

/**
 * @brief Exibe os detalhes de um livro formatados.
 */
void exibir_livro(const Livro *l) {
    int disp = l->exemplares - l->emprestados;
    printf("  --------------------------------------------------\n");
    printf("  CODIGO: %d\n", l->codigo);
    printf("  TITULO: %s\n", l->titulo);
    printf("  AUTOR: %s\n", l->autor);
    printf("  EDITORA: %s\n", l->editora);
    printf("  ANO: %d\n", l->ano_publicacao);
    printf("  EXEMPLARES TOTAIS: %d\n", l->exemplares);
    printf("  EXEMPLARES EMPRESTADOS: %d\n", l->emprestados);
    printf("  STATUS: %s (%d disponivel)\n", (disp > 0) ? "DISPONIVEL" : "ESGOTADO", disp);
    printf("  VEZES EMPRESTADO: %d\n", l->vezes_emprestado);
    printf("  --------------------------------------------------\n");
}

/**
 * @brief Pesquisa livros por código, título ou autor.
 */
void pesquisar_livros() {
    int opcao, cod_busca, encontrados = 0;
    char termo_busca[101];

    printf("\n--- PESQUISAR LIVROS ---\n");
    printf("1. Pesquisar por Codigo\n");
    printf("2. Pesquisar por Titulo\n");
    printf("3. Pesquisar por Autor\n");
    printf("Escolha a opcao: ");

    if (scanf("%d", &opcao) != 1) {
        opcao = -1; // Para evitar que a próxima leitura de string falhe imediatamente
        printf("Opcao invalida.\n");
        // Nenhuma limpeza de buffer
        return;
    }
    // Nenhuma limpeza de buffer

    printf("--------------------------\n");

    switch (opcao) {
        case 1:
            printf("Digite o Codigo: ");
            if (scanf("%d", &cod_busca) == 1) {
                int idx = buscar_livro_por_codigo(cod_busca);
                if (idx != -1) {
                    exibir_livro(&livros[idx]);
                    encontrados++;
                }
            } else {
                printf("Entrada invalida para codigo.\n");
                // Nenhuma limpeza de buffer
            }
            break;
        case 2:
            printf("Digite parte do Titulo: ");
            fgets(termo_busca, 101, stdin);
            termo_busca[strcspn(termo_busca, "\n")] = 0;
            for (int i = 0; i < cont_livros; i++) {
                if (strstr(livros[i].titulo, termo_busca) != NULL) {
                    exibir_livro(&livros[i]);
                    encontrados++;
                }
            }
            break;
        case 3:
            printf("Digite parte do Autor: ");
            fgets(termo_busca, 81, stdin);
            termo_busca[strcspn(termo_busca, "\n")] = 0;
            for (int i = 0; i < cont_livros; i++) {
                if (strstr(livros[i].autor, termo_busca) != NULL) {
                    exibir_livro(&livros[i]);
                    encontrados++;
                }
            }
            break;
        default:
            printf("Opcao de busca invalida.\n");
            return;
    }

    if (encontrados == 0) {
        printf("Nenhum livro encontrado com o criterio fornecido.\n");
    }
}

/**
 * @brief Exibe os detalhes de um usuário formatados.
 */
void exibir_usuario(const Usuario *u) {
    printf("  --------------------------------------------------\n");
    printf("  MATRICULA: %d\n", u->matricula);
    printf("  NOME: %s\n", u->nome);
    printf("  CURSO: %s\n", u->curso);
    printf("  TELEFONE: %s\n", u->telefone);
    printf("  CADASTRO: %02d/%02d/%d\n", u->data_cadastro.dia, u->data_cadastro.mes, u->data_cadastro.ano);
    printf("  --------------------------------------------------\n");
}

/**
 * @brief Pesquisa usuários por matrícula ou nome.
 */
void pesquisar_usuarios() {
    int opcao, mat_busca, encontrados = 0;
    char termo_busca[101];

    printf("\n--- PESQUISAR USUARIOS ---\n");
    printf("1. Pesquisar por Matricula\n");
    printf("2. Pesquisar por Nome\n");
    printf("Escolha a opcao: ");

    if (scanf("%d", &opcao) != 1) {
        opcao = -1;
        printf("Opcao invalida.\n");
        // Nenhuma limpeza de buffer
        return;
    }
    // Nenhuma limpeza de buffer

    printf("--------------------------\n");

    switch (opcao) {
        case 1:
            printf("Digite a Matricula: ");
            if (scanf("%d", &mat_busca) == 1) {
                int idx = buscar_usuario_por_matricula(mat_busca);
                if (idx != -1) {
                    exibir_usuario(&usuarios[idx]);
                    encontrados++;
                }
            } else {
                printf("Entrada invalida para matricula.\n");
                // Nenhuma limpeza de buffer
            }
            break;
        case 2:
            printf("Digite parte do Nome: ");
            fgets(termo_busca, 101, stdin);
            termo_busca[strcspn(termo_busca, "\n")] = 0;
            for (int i = 0; i < cont_usuarios; i++) {
                if (strstr(usuarios[i].nome, termo_busca) != NULL) {
                    exibir_usuario(&usuarios[i]);
                    encontrados++;
                }
            }
            break;
        default:
            printf("Opcao de busca invalida.\n");
            return;
    }

    if (encontrados == 0) {
        printf("Nenhum usuario encontrado com o criterio fornecido.\n");
    }
}

/**
 * @brief Lista todos os empréstimos ativos (status "Ativo").
 */
void listar_emprestimos_ativos() {
    int encontrados = 0;
    printf("\n--- EMPRESTIMOS ATIVOS ---\n");
    printf("Cod.Emp | Matricula | Cod.Livro | Data Emprestimo | Devolucao Prevista\n");
    printf("--------------------------------------------------------------------\n");

    for (int i = 0; i < cont_emprestimos; i++) {
        if (strcmp(emprestimos[i].status, "Ativo") == 0) {
            printf("%7d | %9d | %9d | %02d/%02d/%d      | %02d/%02d/%d\n",
                   emprestimos[i].codigo_emprestimo,
                   emprestimos[i].matricula_usuario,
                   emprestimos[i].codigo_livro,
                   emprestimos[i].data_emprestimo.dia, emprestimos[i].data_emprestimo.mes, emprestimos[i].data_emprestimo.ano,
                   emprestimos[i].data_devolucao_prevista.dia, emprestimos[i].data_devolucao_prevista.mes, emprestimos[i].data_devolucao_prevista.ano);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum emprestimo ativo no momento.\n");
    }
    printf("--------------------------------------------------------------------\n");
}


// --- 4. FUNCIONALIDADES AVANÇADAS (PARTE 5) ---

/**
 * @brief Função de comparação para qsort (ordenar por vezes_emprestado).
 */
int comparar_livros_por_emprestimos(const void *a, const void *b) {
    const Livro *l1 = (const Livro *)a;
    const Livro *l2 = (const Livro *)b;
    // Ordem decrescente
    return l2->vezes_emprestado - l1->vezes_emprestado;
}

/**
 * @brief Relatório de livros mais emprestados.
 */
void relatorio_livros_mais_emprestados() {
    if (cont_livros == 0) {
        printf("\nNenhum livro cadastrado.\n");
        return;
    }

    // Cria uma cópia temporária do vetor de livros para não alterar a ordem original
    Livro temp_livros[MAX_LIVROS];
    memcpy(temp_livros, livros, cont_livros * sizeof(Livro));

    // Ordena a cópia
    qsort(temp_livros, cont_livros, sizeof(Livro), comparar_livros_por_emprestimos);

    printf("\n--- RELATORIO: TOP 10 LIVROS MAIS EMPRESTADOS ---\n");
    printf("Rank | Codigo | Titulo (Ate 40c) | Vezes Emprestado\n");
    printf("---------------------------------------------------\n");

    int limite = (cont_livros < 10) ? cont_livros : 10;

    for (int i = 0; i < limite; i++) {
        char titulo_curto[41];
        strncpy(titulo_curto, temp_livros[i].titulo, 40);
        titulo_curto[40] = '\0'; // Garantir terminação

        printf("%4d | %6d | %-40s | %16d\n",
               i + 1,
               temp_livros[i].codigo,
               titulo_curto,
               temp_livros[i].vezes_emprestado);
    }
    printf("---------------------------------------------------\n");
}

/**
 * @brief Relatório de usuários com empréstimos em atraso.
 */
void relatorio_atrasos() {
    Data hoje = obter_data_atual_simulada();
    int encontrados = 0;

    printf("\n--- RELATORIO: EMPRESTIMOS EM ATRASO (Data Ref: %02d/%02d/%d) ---\n", hoje.dia, hoje.mes, hoje.ano);
    printf("Matricula | Nome do Usuario | Cod.Livro | Titulo do Livro | Devolucao Prevista\n");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < cont_emprestimos; i++) {
        if (strcmp(emprestimos[i].status, "Ativo") == 0) {
            // Verifica se a data de hoje é maior que a data prevista de devolução
            if (comparar_datas(hoje, emprestimos[i].data_devolucao_prevista) > 0) {
                encontrados++;

                // Busca o nome do usuário e título do livro (sem validação de erro para concisão)
                int idx_u = buscar_usuario_por_matricula(emprestimos[i].matricula_usuario);
                int idx_l = buscar_livro_por_codigo(emprestimos[i].codigo_livro);

                printf("%9d | %-15s | %9d | %-15s | %02d/%02d/%d\n",
                       emprestimos[i].matricula_usuario,
                       (idx_u != -1) ? usuarios[idx_u].nome : "N/A",
                       emprestimos[i].codigo_livro,
                       (idx_l != -1) ? livros[idx_l].titulo : "N/A",
                       emprestimos[i].data_devolucao_prevista.dia,
                       emprestimos[i].data_devolucao_prevista.mes,
                       emprestimos[i].data_devolucao_prevista.ano);
            }
        }
    }

    if (encontrados == 0) {
        printf("Nenhum emprestimo ativo esta em atraso na data de referencia.\n");
    }
    printf("--------------------------------------------------------------------------------\n");
}

/**
 * @brief Busca avançada com múltiplos critérios para Livros (Título E Ano).
 */
void busca_avancada_livros() {
    char titulo_busca[101];
    int ano_busca, encontrados = 0;

    printf("\n--- BUSCA AVANCADA DE LIVROS (Titulo E Ano) ---\n");

    // Critério 1: Título
    // O usuário deve garantir que o buffer esteja limpo antes de chamar fgets
    printf("Digite parte do Titulo: ");
    fgets(titulo_busca, 101, stdin);
    titulo_busca[strcspn(titulo_busca, "\n")] = 0;

    // Critério 2: Ano
    printf("Digite o Ano de Publicacao (ou 0 para ignorar o ano): ");
    if (scanf("%d", &ano_busca) != 1) {
        printf("Entrada invalida para ano. Ignorando criterio de ano.\n");
        ano_busca = 0;
    }
    // Nenhuma limpeza de buffer

    printf("\n--- Resultados da Busca ---\n");

    for (int i = 0; i < cont_livros; i++) {
        int match_titulo = (strstr(livros[i].titulo, titulo_busca) != NULL);
        int match_ano = (ano_busca == 0 || livros[i].ano_publicacao == ano_busca);

        if (match_titulo && match_ano) {
            exibir_livro(&livros[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum livro encontrado com os criterios fornecidos.\n");
    }
}


// --- PARTE 2: SISTEMA DE MENUS E CONTROLE DE FLUXO ---

/**
 * @brief Submenu para gerenciar livros.
 */
void menu_livros() {
    int opcao;
    do {
        printf("\n\n--- MENU LIVROS ---\n");
        printf("1. Cadastrar Novo Livro\n");
        printf("2. Pesquisar Livro\n");
        printf("3. Busca Avancada (Titulo E Ano)\n");
        printf("4. Listar Livros Mais Emprestados\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Garante loop para entrada inválida
            // Nenhuma limpeza de buffer
        }
        // Nenhuma limpeza de buffer

        switch (opcao) {
            case 1: cadastrar_livro(); break;
            case 2: pesquisar_livros(); break;
            case 3: busca_avancada_livros(); break;
            case 4: relatorio_livros_mais_emprestados(); break;
            case 0: printf("Voltando ao Menu Principal...\n"); break;
            default: printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

/**
 * @brief Submenu para gerenciar usuários.
 */
void menu_usuarios() {
    int opcao;
    do {
        printf("\n\n--- MENU USUARIOS ---\n");
        printf("1. Cadastrar Novo Usuario\n");
        printf("2. Pesquisar Usuario\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Garante loop para entrada inválida
            // Nenhuma limpeza de buffer
        }
        // Nenhuma limpeza de buffer

        switch (opcao) {
            case 1: cadastrar_usuario(); break;
            case 2: pesquisar_usuarios(); break;
            case 0: printf("Voltando ao Menu Principal...\n"); break;
            default: printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

/**
 * @brief Submenu para gerenciar empréstimos.
 */
void menu_emprestimos() {
    int opcao;
    do {
        printf("\n\n--- MENU EMPRESTIMOS ---\n");
        printf("1. Realizar Novo Emprestimo\n");
        printf("2. Realizar Devolucao\n");
        printf("3. Renovar Emprestimo\n");
        printf("4. Listar Emprestimos Ativos\n");
        printf("5. Relatorio de Atrasos\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Garante loop para entrada inválida
            // Nenhuma limpeza de buffer
        }
        // Nenhuma limpeza de buffer

        switch (opcao) {
            case 1: realizar_emprestimo(); break;
            case 2: realizar_devolucao(); break;
            case 3: renovar_emprestimo(); break;
            case 4: listar_emprestimos_ativos(); break;
            case 5: relatorio_atrasos(); break;
            case 0: printf("Voltando ao Menu Principal...\n"); break;
            default: printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);
}

/**
 * @brief Função principal para o sistema de gerenciamento.
 */
int main() {
    // 1. Carregar dados ao iniciar o programa
    carregar_dados();

    int opcao;

    do {
        printf("\n\n=============== SISTEMA DE GERENCIAMENTO DE BIBLIOTECA ===============\n");
        printf("                 Acervo: %d Livros | %d Usuarios | %d Emprestimos\n", cont_livros, cont_usuarios, cont_emprestimos);
        printf("--------------------------------------------------------------------\n");
        printf("1. Gerenciar Livros (Cadastro, Pesquisa, Relatorios)\n");
        printf("2. Gerenciar Usuarios (Cadastro, Pesquisa)\n");
        printf("3. Gerenciar Emprestimos (Novo, Devolucao, Renovacao, Atrasos)\n");
        printf("0. Sair do Sistema\n");
        printf("--------------------------------------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Garante loop para entrada inválida
            // Nenhuma limpeza de buffer
        }
        // Nenhuma limpeza de buffer

        switch (opcao) {
            case 1: menu_livros(); break;
            case 2: menu_usuarios(); break;
            case 3: menu_emprestimos(); break;
            case 0: printf("\nEncerrando o sistema. Dados salvos com o ultimo backup.\n"); break;
            default: printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}