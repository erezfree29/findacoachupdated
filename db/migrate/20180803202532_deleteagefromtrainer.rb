class Deleteagefromtrainer < ActiveRecord::Migration[5.2]
  def change

    remove_column :trainers, :age
  end
end
