using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

/*
    TODO:
    - Create comparator for Type:
        Paper beats Rock
        Rock beats Scissor
        Scissor beats Paper
    - Need to smarten up the bot to chase down enemies if close by and recently
    converted.
    - Use Speed ability after switch to chase the enemy?
    - Need to implement pathfinding algorithm so I can calculate the cost
        of eating more pellets vs chasing enemy.
    - Convert pathfinding static functions and map to a class?
    - If pacdudes of the same team are stuck make them move away
*/

/**
 * Grab the pellets as fast as you can!
 **/
class Player
{
    static void Main(string[] args)
    {
        #region Map Initialization

        string[] inputs;
        inputs = Console.ReadLine().Split(' ');
        int width = int.Parse(inputs[0]); // size of the grid
        int height = int.Parse(inputs[1]); // top left corner is (x=0, y=0)

        /* contains the walls, pacDudes, and pellets in the game */
        Map map = new Map(width, height);

        for (int i = 0; i < height; i++)
        {
            string row = Console.ReadLine(); // one line of the grid: space " " is floor, pound "#" is wall
            for (int j = 0; j < width; ++j) {
                switch (row[j]) {
                    case ' ':
                        map.PlaceObstacle(j, i, Obstacle.F);
                        break;
                    case '#':
                        map.PlaceObstacle(j, i, Obstacle.W);
                        break;
                }
            }
        }

        #endregion

        // game loop
        while (true)
        {
            #region Game RunTime Variables
            
            inputs = Console.ReadLine().Split(' ');
            int myScore = int.Parse(inputs[0]);
            int opponentScore = int.Parse(inputs[1]);
            int visiblePacCount = int.Parse(Console.ReadLine()); // all your pacs and enemy pacs in sight

            var pacDudes = new List<PacMan>();
            var enemyPacDudes = new List<PacMan>();
            var pellets = new List<Pellet>();
            var commands = new StringBuilder();

            /* We need a shallow copy of the map to keep track of pacDudes and pellets */
            var current_map = map.Clone();

            #endregion

            #region pacDudes Initialization

            for (int i = 0; i < visiblePacCount; i++)
            {
                inputs = Console.ReadLine().Split(' ');

                var pacMan = new PacMan(
                    int.Parse(inputs[0]), // pacId
                    inputs[1] != "0", // is it mine? 
                    new Position(int.Parse(inputs[2]), int.Parse(inputs[3])), // x, y coordinates
                    inputs[4], // pacType: Rock, Paper, Scissors
                    int.Parse(inputs[5]), // Speed Turns Left 
                    int.Parse(inputs[6]) // Ability Cooldown 
                );

                /* track visited tiles */
                map.PlaceObstacle(
                    pacMan.Position.x, 
                    pacMan.Position.y, 
                    Obstacle.V);
                
                current_map[pacMan.Position.x, pacMan.Position.y] = 
                    pacMan.Mine ? Obstacle.M : Obstacle.E;

                if (pacMan.Mine) {
                    pacDudes.Add(pacMan);
                } else {
                    enemyPacDudes.Add(pacMan);
                }
            }

            #endregion

            #region pellets Initialization

            int visiblePelletCount = int.Parse(Console.ReadLine()); // all pellets in sight
            for (int i = 0; i < visiblePelletCount; i++)
            {
                inputs = Console.ReadLine().Split(' ');
                
                var pellet = new Pellet(
                    new Position(
                        int.Parse(inputs[0]), // x
                        int.Parse(inputs[1])  // y
                    ),
                    int.Parse(inputs[2]) // amount of points this pellet is worth
                );

                current_map[
                    pellet.Position.x, 
                    pellet.Position.y
                ] = Obstacle.P;

                pellets.Add(pellet);
            }

            #endregion

            /* Print the map for debugging purposes */
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; ++j) Console.Error.Write(current_map[j, i]);
                Console.Error.WriteLine();
            }

            #region PacDudes Processing 

            foreach(var pacman in pacDudes) {
                Position pos = pacman.Position;

                PacMan enemy = null;
                int enemyDistance = FindClosestObject(pos, enemyPacDudes, ref enemy);

                Pellet pellet = null;
                int pelletDistance = FindClosestObject(pos, pellets, ref pellet);

                /* Pass in grid to map class to process it */
                Map game_map = new Map(width, height, current_map);
                string command = pacman.GetCommand(enemy, enemyDistance, pellet, pelletDistance, game_map, width, height);

                if (commands.Length == 0) commands.Append(command);
                else commands.AppendFormat("|{0}", command);
            }

            #endregion

            /* Send the command */
            Console.WriteLine(commands.ToString());

        }
    }

    /*
        TODO: Maybe add this into a interface or something? Instead of repeating it?
    */
    static int FindClosestObject(Position pos, List<PacMan> objects, ref PacMan target) {
        int shortestDistance = 100;

        foreach(var obj in objects) {
            int distance = pos.GetDistance(obj.Position);
            shortestDistance = Math.Min(shortestDistance, distance);
            if (distance == shortestDistance) {
                target = obj;
            }
        }

        return shortestDistance;
    }
    static int FindClosestObject(Position pos, List<Pellet> objects, ref Pellet target) {
        int shortestDistance = 100;

        foreach(var obj in objects) {
            int distance = pos.GetDistance(obj.Position);
            shortestDistance = Math.Min(shortestDistance, distance);
            if (distance == shortestDistance) {
                target = obj;
            }
        }

        return shortestDistance;
    }

    #region Game Objects

    class Map {
        private Obstacle[,] m_grid;
        private int m_width;
        private int m_height;
        public Map(int width, int height) {
            m_grid = new Obstacle[width, height];
            m_width = width;
            m_height = height;
        }
        /* This is useful when the grid is known */
        public Map(int width, int height, Obstacle[,] grid) {
            m_grid = grid.Clone() as Obstacle[,];
            m_width = width;
            m_height = height;
        }
        public Obstacle[,] Grid {
            get { return m_grid; }
        }
        public int Width {
            get { return m_width; }
        }
        public int Height {
            get { return m_height; }
        }
        public Obstacle GetObstacle(Position pos) {
            if (pos.x < 0 || pos.x >= m_width) 
                throw new Exception("The x coordinate cannot be larger/smaller than the width of the map!");
            if (pos.y < 0 || pos.y >= m_height)
                throw new Exception("The y coordinate cannot be larger/smaller than the height of the map!");
            
            return m_grid[pos.x, pos.y];
        }
        public void PlaceObstacle(Position pos, Obstacle obs) {
            if (pos.x < 0 || pos.x >= m_width) 
                throw new Exception("The x coordinate cannot be larger/smaller than the width of the map!");
            if (pos.y < 0 || pos.y >= m_height)
                throw new Exception("The y coordinate cannot be larger/smaller than the height of the map!");
            
            m_grid[pos.x, pos.y] = obs;
        }
        public void PlaceObstacle(int x, int y, Obstacle obs) {
            if (x < 0 || x >= m_width) 
                throw new Exception("The x coordinate cannot be larger/smaller than the width of the map!");
            if (y < 0 || y >= m_height)
                throw new Exception("The y coordinate cannot be larger/smaller than the height of the map!");
            
            m_grid[x, y] = obs;
        }
        public Obstacle[,] Clone() {
            return m_grid.Clone() as Obstacle[,];
        }
        /*
            Based on excellent source:
                https://www.redblobgames.com/pathfinding/grids/graphs.html
        */
        private List<Position> GetNeighbours(Position pos) {
            var directions = new List<Position> {
                new Position(1, 0), // Right
                new Position(0, 1), // Up
                new Position(-1, 0), // Left
                new Position(0, -1) // Down
            };

            var result = new List<Position>();

            foreach(var direction in directions) {
                Position neighbour = 
                    new Position(
                        direction.x + pos.x, 
                        direction.y + pos.y);
                
                /* 
                    If the coordinates are beyond the maximum or minimum
                    let's wrap around!
                */
                if (neighbour.x >= m_width) neighbour.x = 0;
                else if (neighbour.x < 0) neighbour.x = m_width - 1;

                if (neighbour.y >= m_height) neighbour.y = 0;
                else if (neighbour.y < 0) neighbour.y = m_height - 1;

                /* If the neighbour is a wall, then ignore it */
                if (this.GetObstacle(neighbour) == Obstacle.W) continue;

                result.Add(neighbour);
            }

            return result;
        }

        /*
            Simple Breadth-First Search (BFS) to search for closest valid coordinate

            Based on genius source:
                https://www.redblobgames.com/pathfinding/a-star/introduction.html
        */
        public Position BFS(
            Position start
        ) {
            var frontier = new Queue<Position>();
            var cameFrom = new Dictionary<string, Position>();
            cameFrom.Add(start.ToString(), null);
            Position goal = null;

            frontier.Enqueue(start);
            while (frontier.Count !=0) {
                var current = frontier.Dequeue();

                Obstacle obstacle = m_grid[current.x, current.y];

                if (
                    //obstacle == Obstacle.E ||
                    obstacle == Obstacle.F ||
                    obstacle == Obstacle.P
                ) {
                        goal = current;
                        break;
                }

                foreach(var next in GetNeighbours(current)) {
                    if (!cameFrom.ContainsKey(next.ToString())) {
                        frontier.Enqueue(next);
                        cameFrom.Add(next.ToString(), current);
                    }
                }
            }

            return goal;
        }
    }

    class PacMan {
        #region Internal Variables

        private int m_pacId;
        private bool m_mine;
        private Position m_position;
        private PacType m_type;
        private int m_speedTurnsLeft;
        private int m_abilityCooldown;

        #endregion

        #region Constructor
        public PacMan(
            int pacId, bool mine, Position position, string type, 
            int speedTurnsLeft, int abilityCooldown
        ) {

            m_pacId = pacId;
            m_mine = mine;
            m_position = position;
            m_speedTurnsLeft = speedTurnsLeft;
            m_abilityCooldown = abilityCooldown;

            switch(type) {
                case "ROCK":
                    m_type = PacType.Rock;
                    break;
                case "PAPER":
                    m_type = PacType.Paper;
                    break;
                case "SCISSORS":
                    m_type = PacType.Scissors;
                    break; 
            }
        }

        #endregion

        #region Properties

        public int PacId {
            get { return m_pacId; }
        }
        public bool Mine {
            get { return m_mine; }
        }
        public Position Position {
            get { return m_position; }
        }
        public PacType Type {
            get { return m_type; }
        }
        public int SpeedTurnsLeft {
            get { return m_speedTurnsLeft; }
        }
        public int AbilityCooldown {
            get { return m_abilityCooldown; }
        }

        #endregion

        #region Functions

        public string MoveTo(Position target) {            
            return string.Format("MOVE {0} {1} {2}", m_pacId, target.x, target.y);
        }
        public string Switch(string counterType) {
            
            return string.Format("SWITCH {0} {1}", m_pacId, counterType);
        }
        public string Speed() {
            return string.Format("SPEED {0}", m_pacId);
        }
        /* 
            TODO:
            - Cases to cover:
                - If no enemy is nearby:
                    - Catch the closest pellets
                    - If Sped up and Switched but enemy is no longer nearby, focus on pellets
                - If enemy is nearby:
                    - Switch type to counter enemy -- Set Switch flag to true -- REMEMBER cool down is 10 turns!
                    - If already switched and ability cooled-down 
                        - Check if PacDude can beat enemy
                            - If it can Speed up and chase enemy -- We'll need a flag for Switch such that it's false after speed up
                    - If already switched but still cooling-down, catch the closest pellets
                    - If PacDude is  still in the same coordinate then move away... It means we've clashed with another pacDude!
            - Within this function we'd have to implement the pathfinding function
        */
        /* This is the function where all the decision making is to be made */
        public string GetCommand(PacMan enemy, int enemyDistance, Pellet pellet, int pelletDistance, Map map, int width, int height) {
            
            /* If we're cooled down then let's put our abilities to good use! */
            if ((m_abilityCooldown == 0) && (m_speedTurnsLeft == 0)) return Speed();

            if (enemy != null && enemyDistance <= 3 && m_abilityCooldown == 0) {
                string counterType = CounterPacType(enemy.Type);
                if (!counterType.Equals(m_type)) return this.Switch(counterType);
            }

            if (pellet == null) {
                Position next = map.BFS(m_position);
                return next == null ? this.MoveTo(m_position) : this.MoveTo(next);
            } else {
                return this.MoveTo(pellet.Position);
            }
        }
        
        /* 
            Return the type to counter the enemy's type
        */
        private string CounterPacType(PacType enemyType) {
            switch (enemyType) {
                case PacType.Rock:
                    return "PAPER";
                case PacType.Paper:
                    return "SCISSORS";
                case PacType.Scissors:
                default:
                    return "ROCK";
            }
        }

        #endregion
    }

    class Pellet {
        private Position m_position;
        private int m_value;

        public Pellet(Position position, int value) {
            m_position = position;
            m_value = value;
        }
        public Position Position {
            get { return m_position; }
        }
        public int Value {
            get { return m_value; }
        }
    }

    class Position : IComparable<Position> {
        private int m_x;
        private int m_y;

        public Position(int x, int y) {
            m_x = x;
            m_y = y;
        }
        public int x {
            get { return m_x; }
            set { m_x = value; }
        }
        public int y {
            get { return m_y; }
            set { m_y = value; }
        }

        public int GetDistance (Position final) {
            /* Manhattan distance */
            return Math.Abs(final.x - m_x) + Math.Abs(final.y - m_y);
        }

        public int CompareTo(Position other) {
            if (m_x == other.x && m_y == other.y) return 0;
            else return this.GetDistance(other);
        }

        public override string ToString() {
            return string.Format("({0},{1})", m_x, m_y);
        }

    }

    /*
        Enumeration of the different types of "obstacles"
        in the map.

        This will become useful later on to determine the costs during path-finding.
    */
    public enum Obstacle {
        F, /* Floor */
        P, /* Pellet */
        W, /* Wall */
        M, /* MyPacDude(s) */
        E, /* EnemyPacDude(s) */
        V /* Visited */
    }

    public enum PacType {
        Rock,
        Paper,
        Scissors
    }

    #endregion

}
