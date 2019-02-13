#define BUFFER_SIZE 9

typedef struct
{
    char buffer[BUFFER_SIZE];
    unsigned first_idx;
    unsigned count;
    int prod[2];
    int con[2];
} Queue;

Queue *init(Queue *queue)
{
    queue->count = 0;
    queue->first_idx = 0;
    queue->prod[0] = 0;
    queue->prod[1] = 0;
    queue->con[0] = 0;
    queue->con[1] = 0;

    return queue;
}

int push(Queue *queue, char letter)
{
    if (queue->count <= BUFFER_SIZE)
    {
        char tmp = queue->buffer[queue->first_idx];
        queue->buffer[(queue->first_idx + queue->count) % BUFFER_SIZE] = letter;
        queue->count++;
    }

    return queue->count;
}

char pop(Queue *queue)
{
    if (queue->count > 0)
    {
        char tmp = queue->buffer[queue->first_idx];
        queue->count--;
        queue->first_idx = (queue->first_idx + 1) % BUFFER_SIZE;

        return tmp;
    }

    return '\0';
}

void print(Queue *queue)
{
    printf("Q: ");
    for (int i = 0; i < queue->count; i++)
    {
        printf("%c ", queue->buffer[((queue->first_idx) + i) % BUFFER_SIZE]);
    }
    printf("\n");
}