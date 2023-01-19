import os
import sys
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"
import pygame
import math
import random


DARKGREEN = (0,100,0)
GREEN = (0,255,0)
RED = (255,0,0)
BLACK =(0,0,0)
BROWN = (139,69,19)
WHITE=(255,255,255)
YELLOW=(255,255,0)

portal_block_radius = 2
snake_speed = 15
width = 750
block_width = 25
rows = round(width / block_width)
start = round(rows / 4)
header = 50
eye_radius = 3
border_width = 5
centre = round(block_width/2)


pygame.init()

win = pygame.display.set_mode((width, width+header))
        
score_font = pygame.font.SysFont("agencyfb", 35) # agencyfb, bahnschrift 

color_dict = {
    'Head': GREEN,
    "Body": GREEN,
    "Apple": RED
}

class Block():

    def __init__(self, x, y, type="Body"):
        self.x = x
        self.y= y
        self.type = type
    
    def drawBlock(self):
        start_width = self.x * block_width
        start_height = self.y * block_width
        pygame.draw.rect(win, color_dict.get(self.type, GREEN), (start_width,start_height, block_width, block_width))
        
        if self.type == "Apple":
            petiole =[
                (start_width + centre - eye_radius, start_height - 2 * eye_radius),
                (start_width + centre + eye_radius, start_height - 2 * eye_radius),
                (start_width + centre + eye_radius, start_height + eye_radius),
                (start_width + centre - eye_radius, start_height + eye_radius),
            ]
            pygame.draw.polygon(win, BROWN, petiole, 0)
        elif self.type == "Head":
            left_eye=[
                (start_width + centre - eye_radius, start_height + centre - eye_radius),
                (start_width + centre - eye_radius, start_height + centre + eye_radius)
            ]
            right_eye=[
                (start_width + centre + eye_radius, start_height + centre -eye_radius),
                (start_width + centre + eye_radius, start_height + centre + eye_radius)
            ]
            tounge =[
                (start_width + centre - eye_radius, (self.y + 1) * block_width - eye_radius),
                (start_width + centre + eye_radius, (self.y + 1) * block_width - eye_radius),
                (start_width + centre + eye_radius, (self.y + 1) * block_width + 2 * eye_radius),
                (start_width + centre, (self.y + 1) * block_width + eye_radius),
                (start_width + centre - eye_radius, (self.y + 1) * block_width + 2 * eye_radius),
            ]
            pygame.draw.line(win, BLACK, left_eye[0], left_eye[1], 2)
            pygame.draw.line(win, BLACK, right_eye[0], right_eye[1], 2)
            pygame.draw.polygon(win, RED, tounge, 0)

 
class Snake():
    
    def __init__(self, color, x, y):
        self.color = color
        self.head = Block(x, y, type="Head")
        self.body = []
        self.dirx = 1
        self.diry = 0
        self.future_tail = [x - self.dirx,  y - self.diry]
 
    def move(self):
        old_dirx = self.dirx
        old_diry = self.diry
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                   if old_dirx != 1:
                        self.dirx = -1
                        self.diry = 0
                elif event.key == pygame.K_RIGHT:
                    if old_dirx != -1:
                        self.dirx = 1
                        self.diry = 0
                elif event.key == pygame.K_UP:
                    if old_diry != 1:
                        self.dirx = 0
                        self.diry = -1
                elif event.key == pygame.K_DOWN:
                    if old_diry != -1:
                        self.dirx = 0
                        self.diry = 1                   
    
        coordinates_t1=[self.head.x, self.head.y]
        self.head.x=(self.head.x + self.dirx)%rows
        self.head.y=(self.head.y + self.diry)%rows

        for i, part in enumerate(self.body):
            coordinates_t0 = [part.x, part.y]
            self.body[i].x=coordinates_t1[0]
            self.body[i].y=coordinates_t1[1]
            coordinates_t1=coordinates_t0
        self.future_tail = coordinates_t1
        
 
 
    def addTailToBody(self):
        self.body.append(Block(self.future_tail[0], self.future_tail[1]))

    def drawSnake(self):
        for body in self.body:
            body.drawBlock()
        self.head.drawBlock()        



def drawHeader():
    pygame.draw.line(win, WHITE, (0,width),(width,width))
        

def drawBorder():
    upperBorder=[
        (0,0),
        (width, 0),
        (width, (centre - portal_block_radius) * block_width),
        (width - border_width, (centre - portal_block_radius) * block_width),
        (width - border_width, border_width),
        (border_width, border_width),
        (border_width, (centre - portal_block_radius) * block_width),
        (0, (centre - portal_block_radius) * block_width),
    ]
    lowerBorder=[
        (0, width),
        (width, width),
        (width, (centre + portal_block_radius + 1) * block_width),
        (width - border_width, (centre + portal_block_radius + 1) * block_width),
        (width - border_width, width - border_width),
        (border_width, width - border_width),
        (border_width, (centre + portal_block_radius + 1) * block_width),
        (0, (centre + portal_block_radius + 1) * block_width),
    ]
    pygame.draw.polygon(win, BROWN, upperBorder, 0)
    pygame.draw.polygon(win, BROWN, lowerBorder, 0)
    
 
 
def generateRandomApplePosition():
    touching = True
    while touching:
        x = random.randrange(rows)
        y = random.randrange(rows)
        if (x, y) not in list([(b.x,b.y) for b in snake.body]) and (x,y) != (snake.head.x,snake.head.y):
            touching=False
    return [x, y]

 

def displayScore(score, height=0):
    value = score_font.render("Your Score: " + str(score), True, WHITE)
    win.blit(value, [0, height])


def displayGameOverMessage(msg, color):
    fontMsg = score_font.render(msg, True, color)
    win.blit(fontMsg, [width / 4, width / 3]) 

def gameLoop():
    global snake, apple
    
    snake = Snake(GREEN, start, start)
    x, y = generateRandomApplePosition()
    apple = Block(x, y, type="Apple")       
 
    clock = pygame.time.Clock()
    
    while True:
        snake_has_hit_barier = False 
        clock.tick(snake_speed+round(len(snake.body)/2))
        snake.move()

        #check if game is over
        if snake.body != []:
            if abs(snake.head.x- snake.body[0].x) == (rows - 1) and \
                (snake.head.y < centre - portal_block_radius or snake.head.y > centre + portal_block_radius):
                snake_has_hit_barier = True
            elif abs(snake.head.y- snake.body[0].y) == (rows - 1):
                snake_has_hit_barier = True
        else:
            if abs(snake.head.x- snake.future_tail[0]) == (rows - 1) and \
                (snake.head.y < centre - portal_block_radius or snake.head.y > centre + portal_block_radius):
                snake_has_hit_barier = True
            elif abs(snake.head.y- snake.future_tail[1]) == (rows - 1):
                snake_has_hit_barier = True
        if (snake.head.x, snake.head.y) in list([(b.x,b.y) for b in snake.body]) or snake_has_hit_barier:
            displayGameOverMessage("Game over! Press R key to restart", WHITE)
            pygame.display.update()
            game_is_over=True
            while game_is_over:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        pygame.quit()
                        sys.exit()
                    if event.type == pygame.KEYDOWN:
                        if event.key == pygame.K_r:
                            gameLoop()
        
        # check if snake ate the apple
        if (snake.head.x, snake.head.y) == (apple.x, apple.y):
            snake.addTailToBody()
            x, y = generateRandomApplePosition()
            apple = Block(x, y, type="Apple") 
            
        win.fill(DARKGREEN)
        apple.drawBlock()
        drawBorder()
        snake.drawSnake()
        drawHeader()
        displayScore(len(snake.body), width)
        pygame.display.update()

gameLoop()