int tpm_binary_search (cvector_vector_type(tpm_task_t *) tpm_tasks, char *name) {
    int size = cvector_size(tpm_tasks);
    int bottom = 0, mid = 0;
    int top = size - 1;

    while (bottom <= top) {
        mid = bottom + (top - bottom) / 2;
        printf("name = %s, mid = %d, top = %d, bottom = %d\n", name, mid, top, bottom);
        if (strcmp(tpm_tasks[mid]->name, name) == 0) {
            return mid + 1;
        } else if (strcmp(tpm_tasks[mid]->name, name) > 0) {
            top = mid - 1;
        } else {
            bottom = mid + 1;
        }
    }
}
