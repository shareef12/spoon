#include "gui.h"

GtkWidget *window;
GtkWidget *frame;
GtkWidget *pageSize_label, *margin_label;
GtkWidget *pageSize_entry, *margin_entry;
GtkWidget *run, *exif;
GtkWidget *inFile_label, *outDir_label;
GtkWidget *inFile_entry, *outDir_entry;
GtkWidget *inFile_chooser, *outDir_chooser;
GtkWidget *view;
GtkWidget *scrolledWindow;
GtkTextBuffer *buffer;
GtkTextIter iter;

void run_spoon() {
    const char *file = gtk_entry_get_text(GTK_ENTRY(inFile_entry));
    const char *out = gtk_entry_get_text(GTK_ENTRY(outDir_entry));
    int pagesize = atoi(gtk_entry_get_text(GTK_ENTRY(pageSize_entry)));
    int margin = atoi(gtk_entry_get_text(GTK_ENTRY(margin_entry)));
    char recvbuf[512];
    memset(recvbuf, 0, 512);

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    int cmdLen = strlen(file) + strlen(out) + 64;
    char cmd[cmdLen];
    snprintf(cmd, cmdLen, "./spoon -f %s -o %s -p %d -m %d -g %d", file, out, pagesize, margin, pipefd[1]);
    
    pid_t cpid;
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        close(pipefd[0]);       // child doesn't read from pipe
        system(cmd);
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    else {
        close(pipefd[1]);       // parent doesn't write to pipe

        char c, *buf = recvbuf;
        while (read(pipefd[0], &c, 1)) {
            *buf++ = c;
            if (c == '\n') {
                // printf("got line: %s", recvbuf);
                gui_write((char *)recvbuf);
                memset(recvbuf, 0, 512);
                buf = recvbuf;
            }
        }
        gui_write("\n");
    }
}

void run_exif() {

}

void gui_write(char *line) {
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, line, -1, "all", NULL);
    while (gtk_events_pending()) gtk_main_iteration();
    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledWindow));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
}

void update_inFile() {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(inFile_chooser));
    gtk_entry_set_text(GTK_ENTRY(inFile_entry), filename);
}

void update_outDir() {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(outDir_chooser));
    gtk_entry_set_text(GTK_ENTRY(outDir_entry), filename);
}

int main(int argc, char *argv[]) {
    
    gtk_init(&argc, &argv);
    char cwd[1024];
    memset(cwd, 0, 1024);   
 
    if (getcwd(cwd, 1024) == NULL) {
        perror("Failed to get current working directory");
        cwd[0] = '.';
    }

    // window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Spooning for all your needs");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    frame = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), frame);

    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // file IO
    inFile_label = gtk_label_new("Input File");
    outDir_label = gtk_label_new("Output Directory");
    gtk_fixed_put(GTK_FIXED(frame), inFile_label, 20, 60);
    gtk_fixed_put(GTK_FIXED(frame), outDir_label, 20, 95);

    inFile_entry = gtk_entry_new();
    outDir_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(inFile_entry), 50);
    gtk_entry_set_text(GTK_ENTRY(inFile_entry), "~/spoon/images/real.img");
    gtk_entry_set_width_chars(GTK_ENTRY(outDir_entry), 50);
    gtk_entry_set_text(GTK_ENTRY(outDir_entry), cwd);
    gtk_fixed_put(GTK_FIXED(frame), inFile_entry, 140, 60);
    gtk_fixed_put(GTK_FIXED(frame), outDir_entry, 140, 95);

    inFile_chooser = gtk_file_chooser_button_new("Input File", GTK_FILE_CHOOSER_ACTION_OPEN);
    outDir_chooser = gtk_file_chooser_button_new("Output Directory", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_fixed_put(GTK_FIXED(frame), inFile_chooser, 560, 60);
    gtk_fixed_put(GTK_FIXED(frame), outDir_chooser, 560, 95);
    
    g_signal_connect(inFile_chooser, "file-set", G_CALLBACK(update_inFile), NULL);
    g_signal_connect(outDir_chooser, "file-set", G_CALLBACK(update_outDir), NULL);

    // run/exif
    run = gtk_button_new_with_label("Run");
    exif = gtk_button_new_with_label("Exif");
    gtk_widget_set_size_request(run, 80, 35);
    gtk_widget_set_size_request(exif, 80, 35);
    gtk_fixed_put(GTK_FIXED(frame), run, 680, 50);
    gtk_fixed_put(GTK_FIXED(frame), exif, 680, 95);

    g_signal_connect(run, "clicked", G_CALLBACK(run_spoon), NULL);
    g_signal_connect(exif, "clicked", G_CALLBACK(run_exif), NULL);

    // view
    scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolledWindow), view);
    gtk_widget_set_size_request(scrolledWindow, 550, 400);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
    gtk_text_buffer_create_tag(buffer, "all", "pixels_above_lines", 2, "left_margin", 6, "font", "monospace 10", NULL);
    gtk_text_buffer_create_tag(buffer, "center", "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_fixed_put(GTK_FIXED(frame), scrolledWindow, 25, 150);


    // options
    pageSize_label = gtk_label_new("Pagesize");
    margin_label = gtk_label_new("Margin");
    gtk_fixed_put(GTK_FIXED(frame), pageSize_label, 600, 260);
    gtk_fixed_put(GTK_FIXED(frame), margin_label, 600, 295);

    pageSize_entry = gtk_entry_new();
    margin_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(pageSize_entry), 6);
    gtk_entry_set_width_chars(GTK_ENTRY(margin_entry), 6);
    gtk_entry_set_text(GTK_ENTRY(pageSize_entry), "16384");
    gtk_entry_set_text(GTK_ENTRY(margin_entry), "2048");
    gtk_fixed_put(GTK_FIXED(frame), pageSize_entry, 680, 260);
    gtk_fixed_put(GTK_FIXED(frame), margin_entry, 680, 295);


    gtk_widget_show_all(window);

    gtk_main();
    
    return 0;
}   
