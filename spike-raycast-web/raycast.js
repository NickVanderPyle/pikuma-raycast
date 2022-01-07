const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);

const WALL_STRIP_WIDTH = 10;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;

const MINIMAP_SCALE_FACTOR = 0.2;

class Map {
    constructor() {
        this.grid = [
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
            [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
        ];
    }

    render() {
        for (let i = 0; i < MAP_NUM_ROWS; i++) {
            for (let j = 0; j < MAP_NUM_COLS; j++) {
                const tileX = j * TILE_SIZE;
                const tileY = i * TILE_SIZE;
                const tileColor = this.grid[i][j] === 1 ? "#222" : "#fff";
                stroke("#222");
                fill(tileColor);
                rect(
                    MINIMAP_SCALE_FACTOR * tileX,
                    MINIMAP_SCALE_FACTOR * tileY,
                    MINIMAP_SCALE_FACTOR * TILE_SIZE,
                    MINIMAP_SCALE_FACTOR * TILE_SIZE);
            }
        }
    }

    hasWallAt(x, y) {
        if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
            return true;
        }
        const mapGridIndexX = Math.floor(x / TILE_SIZE);
        const mapGridIndexY = Math.floor(y / TILE_SIZE);
        return this.grid[mapGridIndexY][mapGridIndexX] !== 0;
    }
}

class Player {
    constructor() {
        this.x = WINDOW_WIDTH / 2;
        this.y = WINDOW_HEIGHT / 2;
        this.radius = 3;
        this.turnDirection = 0; //-1 if left, +1 if right.
        this.walkDirection = 0; //-1 backwards, +1 if forwards.
        this.rotationAngle = Math.PI / 2;
        this.moveSpeed = 2.0;
        this.rotationSpeed = 2 * (Math.PI / 180);
    }

    render() {
        //player
        noStroke();
        fill("red");
        circle(
            MINIMAP_SCALE_FACTOR * this.x,
            MINIMAP_SCALE_FACTOR * this.y,
            MINIMAP_SCALE_FACTOR * this.radius);

        //show where player facing
        stroke("red");
        line(
            MINIMAP_SCALE_FACTOR * this.x,
            MINIMAP_SCALE_FACTOR * this.y,
            MINIMAP_SCALE_FACTOR * (this.x + Math.cos(this.rotationAngle) * 30),
            MINIMAP_SCALE_FACTOR * (this.y + Math.sin(this.rotationAngle) * 30)
        );
    }

    normalizeAngle(angle) {
        angle = angle % (2 * Math.PI);
        if (angle < 0) {
            angle = (2 * Math.PI) + angle;
        }
        return angle;
    }

    update() {
        this.rotationAngle += this.turnDirection * this.rotationSpeed;
        this.rotationAngle = this.normalizeAngle(this.rotationAngle);

        const moveStep = this.walkDirection * this.moveSpeed;
        const newPlayerX = this.x + Math.cos(this.rotationAngle) * moveStep;
        const newPlayerY = this.y + Math.sin(this.rotationAngle) * moveStep;

        if (!grid.hasWallAt(newPlayerX, newPlayerY)) {
            this.x = newPlayerX;
            this.y = newPlayerY;
        }
    }
}

class Ray {
    constructor(rayAngle) {
        this.rayAngle = this.normalizeAngle(rayAngle);
        this.wallHitX = 0;
        this.wallHitY = 0;
        this.distance = 0;
        this.wasHitVertical = false;

        this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
        this.isRayFacingUp = !this.isRayFacingDown;
        this.isRayFacingRight = this.rayAngle < (0.5 * Math.PI) || this.rayAngle > (1.5 * Math.PI);
        this.isRayFacingLeft = !this.isRayFacingRight;
    }

    normalizeAngle(angle) {
        angle = angle % (2 * Math.PI);
        if (angle < 0) {
            angle = (2 * Math.PI) + angle;
        }
        return angle;
    }

    render() {
        stroke("rgba(255, 0, 0, 0.2)");
        line(
            MINIMAP_SCALE_FACTOR * player.x,
            MINIMAP_SCALE_FACTOR * player.y,
            MINIMAP_SCALE_FACTOR * this.wallHitX,
            MINIMAP_SCALE_FACTOR * this.wallHitY
        );
    }

    cast() {
        /*
            Horizontal ray-grid intersection check
        */
        let yIntercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
        yIntercept += this.isRayFacingDown ? TILE_SIZE : 0;
        let xIntercept = player.x + (yIntercept - player.y) / Math.tan(this.rayAngle);

        let yStep = TILE_SIZE;
        yStep *= this.isRayFacingUp ? -1 : 1;

        let xStep = TILE_SIZE / Math.tan(this.rayAngle);
        xStep *= (this.isRayFacingLeft && xStep > 0) ? -1 : 1;
        xStep *= (this.isRayFacingRight && xStep < 0) ? -1 : 1;

        let nextHorizTouchX = xIntercept;
        let nextHorizTouchY = yIntercept;

        let foundHorizontalWallHit = false;
        let horizWallHitX = 0;
        let horizWallHitY = 0;
        while (nextHorizTouchX >= 0 && nextHorizTouchX <= WINDOW_WIDTH
        && nextHorizTouchY >= 0 && nextHorizTouchY <= WINDOW_HEIGHT) {
            if (grid.hasWallAt(nextHorizTouchX, nextHorizTouchY - (this.isRayFacingUp ? 1 : 0))) {
                foundHorizontalWallHit = true;
                horizWallHitX = nextHorizTouchX;
                horizWallHitY = nextHorizTouchY;
                break;
            }

            nextHorizTouchX += xStep;
            nextHorizTouchY += yStep;
        }

        /*
            vertical ray-grid intersection check
        */
        xIntercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
        xIntercept += this.isRayFacingRight ? TILE_SIZE : 0;
        yIntercept = player.y + (xIntercept - player.x) * Math.tan(this.rayAngle);

        xStep = TILE_SIZE;
        xStep *= this.isRayFacingLeft ? -1 : 1;

        yStep = TILE_SIZE * Math.tan(this.rayAngle);
        yStep *= (this.isRayFacingUp && yStep > 0) ? -1 : 1;
        yStep *= (this.isRayFacingDown && yStep < 0) ? -1 : 1;

        let nextVertTouchX = xIntercept;
        let nextVertTouchY = yIntercept;

        let foundVerticalWallHit = false;
        let vertWallHitX = 0;
        let vertWallHitY = 0;
        while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH
        && nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) {
            if (grid.hasWallAt(nextVertTouchX - (this.isRayFacingLeft ? 1 : 0), nextVertTouchY)) {
                foundVerticalWallHit = true;
                vertWallHitX = nextVertTouchX;
                vertWallHitY = nextVertTouchY;
                break;
            }

            nextVertTouchX += xStep;
            nextVertTouchY += yStep;
        }

        const horizHitDistance = (foundHorizontalWallHit)
            ? this.distanceBetweenPoints(player.x, player.y, horizWallHitX, horizWallHitY)
            : Number.MAX_VALUE;
        const vertHitDistance = (foundVerticalWallHit)
            ? this.distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
            : Number.MAX_VALUE;

        if (horizHitDistance < vertHitDistance) {
            this.wallHitX = horizWallHitX;
            this.wallHitY = horizWallHitY;
            this.distance = horizHitDistance;
            this.wasHitVertical = true;
        } else {
            this.wallHitX = vertWallHitX;
            this.wallHitY = vertWallHitY;
            this.distance = vertHitDistance;
            this.wasHitVertical = false;
        }
    }

    distanceBetweenPoints(x1, y1, x2, y2) {
        return Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
    }
}

const grid = new Map();
const player = new Player();
const rays = [];

function keyPressed() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = +1;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = -1;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = +1;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = -1;
    }
}

function keyReleased() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = 0;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = 0;
    }
}

function castAllRays() {
    let rayAngle = player.rotationAngle - (FOV_ANGLE / 2);
    rays.length = 0;

    for (let col = 0; col < NUM_RAYS; col++) {
        const ray = new Ray(rayAngle);
        rays.push(ray);
        ray.cast();

        rayAngle += FOV_ANGLE / NUM_RAYS;
    }
}

function render3DProjectedWalls() {
    const distanceProjectionPlane = (WINDOW_WIDTH / 2) / Math.tan(FOV_ANGLE / 2);

    for (let i = 0; i < rays.length; i++) {
        const ray = rays[i];
        const correctWallDistance = ray.distance * Math.cos(ray.rayAngle - player.rotationAngle);
        const wallstripHeight = (TILE_SIZE / correctWallDistance) * distanceProjectionPlane;

        const alpha = 1; //map(correctWallDistance, 0, WINDOW_WIDTH, 1, 0);
        const color = ray.wasHitVertical ? 255 : 180;


        fill(`rgba(${color}, ${color}, ${color}, ${alpha})`);
        noStroke();
        rect(
            i * WALL_STRIP_WIDTH,
            (WINDOW_HEIGHT / 2) - (wallstripHeight / 2),
            WALL_STRIP_WIDTH,
            wallstripHeight
        )
    }
}

function setup() {
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);
}

function update() {
    player.update();
    castAllRays();
}

function draw() {
    clear();
    update();

    render3DProjectedWalls();

    grid.render();
    for (ray of rays) {
        ray.render();
    }
    player.render();
}