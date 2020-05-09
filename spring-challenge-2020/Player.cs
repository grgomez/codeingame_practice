using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

/*
    TODO:

    - Add extra input parameters to Pellet class
    - Create comparator for Type:
        Paper beats Rock
        Rock beats Scissor
        Scissor beats Paper
    - Implement function to convert map to graph
    - Implement pathfinding algorithm to calculate optimal path.
        - What's the cost based on? 
    - Need to smarten up the bot to chase down enemies if close by and recently
    converted.
    - Use Speed ability after switch to chase the enemy?
    - Need to implement pathfinding algorithm so I can calculate the cost
        of eating more pellets vs chasing enemy.

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
        Obstacle[,] map = new Obstacle[width, height];

        for (int i = 0; i < height; i++)
        {
            string row = Console.ReadLine(); // one line of the grid: space " " is floor, pound "#" is wall
            for (int j = 0; j < width; ++j) {
                switch (row[j]) {
                    case ' ':
                        map[j, i] = Obstacle.F;
                        break;
                    case '#':
                        map[j, i] = Obstacle.W;
                        break;
                }

                Console.Error.Write(map[j, i]);
            }
            Console.Error.WriteLine();
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
            var current_map = map.Clone() as Obstacle[,];

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
                int x = int.Parse(inputs[0]);
                int y = int.Parse(inputs[1]);
                int value = int.Parse(inputs[2]); // amount of points this pellet is worth
                
                current_map[x, y] = Obstacle.P;
                pellets.Add(new Pellet(new Position(x, y), value));
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
                string command = string.Empty;

                /* Look whether the enemy is nearby and switch if they're close enough! */ 
                int shortest_distance = 100;
                command = string.Empty;
                foreach (var enemy in enemyPacDudes) {
                    int distance = pos.distance(enemy.Position);
                    shortest_distance = Math.Min(shortest_distance, distance);
                    if (distance <= 3) {
                        command = pacman.Switch(enemy.Type);
                        Console.Error.WriteLine("The an enemy is {0} units away", shortest_distance);
                        break;
                    }
                }

                Console.Error.WriteLine("The an enemy is {0} units away", shortest_distance);

                if (string.IsNullOrEmpty(command)) {
                    /* Look for the closest pellet */
                    shortest_distance = 100;
                    Position target = null;
                    foreach(var pellet in pellets) {
                        int distance = pos.distance(pellet.Position);
                        shortest_distance = Math.Min(shortest_distance, distance);
                        if (distance == shortest_distance) {
                            target = pellet.Position;
                        }
                    }

                    command = pacman.MoveTo(target);
                }

                if (commands.Length == 0) commands.Append(command);
                else commands.AppendFormat("|{0}", command);

            }

            #endregion

            /* Send the command */
            Console.WriteLine(commands.ToString());

        }
    }

    #region Game Objects

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
            if (target == null) 
                return string.Format("MOVE {0} {1} {2}", m_pacId, m_position.x, m_position.y);
            
            return string.Format("MOVE {0} {1} {2}", m_pacId, target.x, target.y);
        }
        public string Switch(PacType enemyType) {
            
            return m_abilityCooldown == 0 ? 
                string.Format("SWITCH {0} {1}", m_pacId, CounterPacType(enemyType)) : string.Empty;
        }
        public string Speed() {
            return (m_abilityCooldown == 0) && (m_speedTurnsLeft == 0) ?
                string.Format("SPEED {0}", m_pacId) : string.Empty;
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

    // class MapPosition : Position {
    //     private int m_cost;
    //     public MapPosition(int x, int y) {
    //         m_x = x;
    //         m_y = y;
    //     }
    //     public MapPosition(int x, int y, int cost) {
    //         m_x = x;
    //         m_y = y;
    //         m_cost = m_cost;
    //     }
    //     public int Cost {
    //         get { return m_cost; }
    //         set { m_cost = value; }
    //     }
    // }
    class Position /*: IEquatable<Position> */{
        private int m_x;
        private int m_y;

        public Position(int x, int y) {
            m_x = x;
            m_y = y;
        }
        public int x {
            get { return m_x; }
        }
        public int y {
            get { return m_y; }
        }

        public int distance (Position final) {
            /* Manhattan distance */
            return Math.Abs(final.x - m_x) + Math.Abs(final.y - m_y);
        }

        /*public override bool Equals(object obj)
        {
            if (obj == null) return false;
            Part objAsPosition = obj as Position;
            if (objAsPosition == null) return false;
            else return Equals(objAsPosition);
        }

        public override bool Equals(Position position) {        
            return (m_x = position.x) 
                && (m_y == position.y);
        }

        public override int GetHashCode() => HashCode<int, int>.Combine(x, y);*/
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
        E /* EnemyPacDude(s) */
    }

    public enum PacType {
        Rock,
        Paper,
        Scissors
    }

    #endregion

}
