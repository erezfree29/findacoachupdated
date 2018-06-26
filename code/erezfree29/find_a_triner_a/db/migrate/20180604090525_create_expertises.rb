class CreateExpertises < ActiveRecord::Migration[5.2]
  def change
    create_table :expertises do |t|
      t.string :name
      t.integer :difficulty_level

      t.timestamps
    end
  end
end
